#include "worker.h"
#include <pthread.h>

using namespace std;
using namespace soso;

static const char *job_type_str[] = { //
    "m_worker",                       //
    "s_worker"};

/**
 * @brief ModuleWorker 생성자
 * @param worker_num worker(작업자) 수
 * @param wait_for_ms job(작업) 용처 대기 시간 (millisecond)
 */
WorkerManager::WorkerManager(const std::string &name, int worker_num,
                             int wait_for_ms) {
  int i = 0;
  _name = name;
  _worker_num = worker_num;
  this->_wait_for_ms = chrono::milliseconds(wait_for_ms);

  /// initialize job workers
  /// for multi worker
  for (; i < worker_num; i++) {
    _m_workers.push_back(make_shared<Worker>(this, i, MULTI_WORKER));
    _job_m_Q.push_back(deque<shared_ptr<Job>>());
    _job_m_M.push_back(make_shared<mutex>());
    _job_m_CV.push_back(make_shared<condition_variable>());
  }
  /// for single worker
  _s_worker = make_shared<Worker>(this, i, SINGLE_WORKER);
  _job_s_M = make_shared<mutex>();
  _job_s_CV = make_shared<condition_variable>();
  ///
  _job_id_M = make_shared<mutex>();
}

void WorkerManager::workerThreadFunc(shared_ptr<Worker> worker,
                                     deque<shared_ptr<Job>> *Q,
                                     shared_ptr<mutex> M,
                                     shared_ptr<condition_variable> CV) {
  shared_ptr<Job> job;
  const string worker_name = _name + "_" + job_type_str[worker->getJobType()];

  pthread_setname_np(pthread_self(), worker_name.c_str());

  unique_lock<mutex> lock(*M.get());

  while (true) {
    if (!_running && Q->empty()) {
      break;
    }

    // wait for job wakeup signal
    CV->wait_for(lock, _wait_for_ms);

    while (!Q->empty()) {
#if 0
      cout << "pop start: worker:#" << worker->getWorkerID() << " " << Q->size()
           << "\n";
#endif
      job = Q->front();

#if 0
      size_t affinity_idx;
      /// affinity인 경우 worker ID와 작업 hashcode를 비교하여
      /// worker가 처리해야 할 작업인지 판단
      /// 현재 worker가 처리해야 할 작업이 아닌 경우 작업 큐에서 꺼내지 않고
      /// 다른 worker가 처리할 수 있도록 알려(notify)준다.
      if (job->affinity()) {
        affinity_idx = job->jobHashCode() % _worker_num;
        if (worker->getWorkerID() != affinity_idx) {
          CV->notify_one();
          /// 다른 worker에게 CPU를 양보한다.
          this_thread::yield();
          break;
        }
      }
#endif

      job->setWorkerID(worker->getWorkerID());
      auto handler = job->getHandler();
      handler(worker, job); // call job user defined function

      Q->pop_front();
      worker->incCompletedJobs(); // increment completed job count
#if 0
      cout << "pop end: worker:#" << worker->getWorkerID() << " " << Q->size()
           << "\n";
#endif
    }
  }

  /// worker thread 종료 되었음을 알림
  worker->terminate();
  pthread_exit(NULL);
  return;
}

/**
 * @brief 모든 worker(작업자) thread 종료할 것을 알림
 * @param with_join 모든 worker가 종료될 때 까지 대기할지 여부
 */
void WorkerManager::terminate(bool with_join) { //
  _running = false;
  if (with_join) {
    join();
  }
}

/**
 * @brief worker thread가 모두 종료 될 때 까지 대기
 */
void WorkerManager::join() {
  for (auto worker : _m_workers) {
    while (true) {
      if (worker->finished()) {
        break;
      }
      this_thread::sleep_for(_wait_for_ms);
    }
  }
  while (true) {
    if (_s_worker->finished()) {
      break;
    }
    this_thread::sleep_for(_wait_for_ms);
  }
}

string WorkerManager::workerName() { return _name; }

string WorkerManager::report() {
  string result = "";
  size_t total_job = 0;

  for (auto worker : _m_workers) {
    total_job += worker->getCompletedJobs();
    result += "worker#" + to_string(worker->getWorkerID()) + ": " +
              job_type_str[worker->getJobType()] + ": " +
              to_string(worker->getCompletedJobs()) + "\n";
  }
  total_job += _s_worker->getCompletedJobs();
  result += "worker#" + to_string(_s_worker->getWorkerID()) + ": " +
            job_type_str[_s_worker->getJobType()] + ": " +
            to_string(_s_worker->getCompletedJobs()) + "\n";

  result += "total job: " + to_string(total_job) + "\n";
  return result;
}

/**
 * @brief worker에게 작업 요청
 * @param name
 * @param handler
 * @param type
 * @param affinity
 */
void WorkerManager::addJob(const string &name,    //
                           job_handler_t handler, //
                           JOB_QUEUE_TYPE type, bool affinity) {
  size_t job_id;
  size_t worker_id;
  shared_ptr<condition_variable> CV;
  shared_ptr<mutex> M;
  deque<shared_ptr<Job>> *Q;

  /// select worker
  /// TODO: job 분뱁 RR(Round Robin)
  _job_id_M->lock();
  {
    job_id = _job_count++;
    if (affinity) {
      auto hashcode = hash<string>{}(name);
      worker_id = hashcode % _worker_num;
    } else {
      worker_id = job_id % _worker_num;
    }
  }
  _job_id_M->unlock();

  if (type == MULTI_WORKER) {
    Q = &_job_m_Q[worker_id];
    M = _job_m_M[worker_id];
    CV = _job_m_CV[worker_id];
  } else {
    Q = &_job_s_Q;
    M = _job_s_M;
    CV = _job_s_CV;
  }

  {
    lock_guard<mutex> lock(*M);
    Q->push_back(make_shared<Job>(job_id, handler, affinity));
    /// worker에게 작업이 추가되었음을 알림
    CV->notify_one();
  }
}

/**
 * @brief single worker에게 작업 요청
 * @param name
 * @param handler
 */
void WorkerManager::addJobSingleWorker(const string &name, //
                                       job_handler_t handler) {
  return addJob(name, handler, SINGLE_WORKER, false);
}

/**
 * @brief 여러 worker 중 하나의 worker에게 작업 요청
 * @param name
 * @param handler
 * @param affinity
 */
void WorkerManager::addJobMultiWorker(const string &name,    //
                                      job_handler_t handler, //
                                      bool affinity) {
  return addJob(name, handler, MULTI_WORKER, affinity);
}

/**
 * @brief ModuleWorker 수행
 * @param block YES인 경우 run 함수 block되어 뒤에 실행 코드가 수행 안됨\n
 * FALSE인 경우 run 함수 호출 후 바로 리턴됨
 */
void WorkerManager::run(bool block) {
  /// create job worker thread
  for (auto worker : _m_workers) {
    // thread create and run
    auto th = make_shared<thread>(thread(&WorkerManager::workerThreadFunc, //
                                         this,                             //
                                         worker,                           //
                                         &_job_m_Q[worker->getWorkerID()], //
                                         _job_m_M[worker->getWorkerID()],  //
                                         _job_m_CV[worker->getWorkerID()]));

    _worker_threads.push_back(th);
  }
  auto th = make_shared<thread>(thread(&WorkerManager::workerThreadFunc, //
                                       this,                             //
                                       _s_worker,                        //
                                       &_job_s_Q,                        //
                                       _job_s_M,                         //
                                       _job_s_CV));
  _worker_threads.push_back(th);

  // wait for terminated thread
  for (auto th : _worker_threads) {
    if (block) {
      /// block thread
      if ((*th).joinable()) {
        (*th).join();
      }
    } else {
      /// non block thread
      (*th).detach();
    }
  }
}