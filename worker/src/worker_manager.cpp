#include "worker.h"

using namespace std;
using namespace soso;

void WorkerManager::workerThreadFunc(shared_ptr<Worker> worker) {
  shared_ptr<Job> job;
  size_t job_id = 0;
  JOB_QUEUE_TYPE type = worker->getJobType();
  size_t affinity_idx;

  auto worker_manager = worker->getWorkerManager();

  while (worker_manager->_running) {
    unique_lock<mutex> lock(_job_Q_mutex[type]);

    // wait for job wakeup signal
    if (_wait_for_ms.count() == 0) {
      _job_Q_cv[type].wait(lock);
    } else {
      _job_Q_cv[type].wait_for(lock, _wait_for_ms);
    }

    while (!_job_Q[type].empty()) {
      job = _job_Q[type].front();

      /// affinity인 경우 worker ID와 작업 hashcode를 비교하여
      /// worker가 처리해야 할 작업인지 판단
      /// 현재 worker가 처리해야 할 작업이 아닌 경우 작업 큐에서 꺼내지 않고
      /// 다른 worker가 처리할 수 있도록 알려(notify)준다.
      if (job->affinity()) {
        affinity_idx = job->jobHashCode() % worker_manager->_worker_num;
        if (worker->getWorkerID() != affinity_idx) {
          _job_Q_cv[type].notify_one();
          /// 다른 worker에게 CPU를 양보한다.
          this_thread::yield();
          break;
        }
      }

      job->setJobID(job_id++);
      job->setWorkerID(worker->getWorkerID());
      auto handler = job->getHandler();
      handler(worker, job); // call job user defined function

      _job_Q[type].pop_front();
      worker->incCompletedJobs(); // increment completed job count
    }
  }

  /// worker thread 종료 되었음을 알림
  worker->terminate();
  pthread_exit(NULL);
  return;
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

  lock_guard<mutex> lock(_job_Q_mutex[type]);
  _job_Q[type].push_back(make_shared<Job>(name, handler, affinity));
  /// worker에게 작업이 추가되었음을 알림
  _job_Q_cv[type].notify_one();
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
  for (auto worker : _workers) {
    while (true) {
      if (worker->finished()) {
        break;
      }
      this_thread::sleep_for(_wait_for_ms);
    }
  }
}

/**
 * @brief ModuleWorker 생성자
 * @param worker_num worker(작업자) 수
 * @param wait_for_ms job(작업) 용처 대기 시간 (millisecond)
 */
WorkerManager::WorkerManager(int worker_num, int wait_for_ms) {
  _worker_num = worker_num;
  this->_wait_for_ms = chrono::milliseconds(wait_for_ms);
  int i = 0;

  /// initialize job workers
  for (; i < worker_num; i++) {
    _workers.push_back( //
        make_shared<Worker>(this, i, MULTI_WORKER));
  }
  _workers.push_back( //
      make_shared<Worker>(this, i, SINGLE_WORKER));
}

string WorkerManager::report() {
  string result = "";
  const char *job_type_str[] = {                //
                                "MULTI_WORKER", //
                                "SINGLE_WORKER"};
  for (auto w : _workers) {
    //
    result += "worker#" + to_string(w->getWorkerID()) + ": " +
              job_type_str[w->getJobType()] + ": " +
              to_string(w->getCompletedJobs()) + "\n";
  }
  return result;
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
  for (auto worker : _workers) {
    // thread create and run
    auto th = make_shared<thread>(
        thread(&WorkerManager::workerThreadFunc, this, worker));
    _worker_threads.push_back(th);
  }

  // wait for terminated thread
  for (auto th : _worker_threads) {
    if (block) {
      /// block thread
      (*th).join();
    } else {
      /// non block thread
      (*th).detach();
    }
  }
}