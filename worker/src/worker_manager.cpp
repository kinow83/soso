#include "worker.h"
#include <iostream>
#include <pthread.h>

using namespace std;
using namespace soso;

static const char *job_type_str[] = { //
    "m_worker",                       //
    "s_worker"};

WorkerManager::WorkerManager(const string &name, int worker_num,
                             int wait_for_ms) {
  int i = 0;
  _name = name;
  _worker_num = worker_num;
  _wait_for_ms = chrono::milliseconds(wait_for_ms);

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
}

WorkerManager::~WorkerManager() { //
  terminate();
}

void WorkerManager::terminate() {
  shared_ptr<Worker> W;
  shared_ptr<mutex> M;
  shared_ptr<condition_variable> CV;
  deque<shared_ptr<Job>> Q;

  _running = false;

  if (!_joinable) {
    return;
  }

  // 종료를 위해 wait를 깨운다.
  for (size_t i = 0; i < _worker_num; i++) {
    W = _m_workers[i];
    M = _job_m_M[i];
    CV = _job_m_CV[i];
    Q = _job_m_Q[i];

    while (true) {
      {
        lock_guard<mutex> lock(*M);
        CV->notify_all();
      }
      if (W->finished()) {
        break;
      }
      this_thread::sleep_for(_wait_for_ms);
    }
  }

  W = _s_worker;
  M = _job_s_M;
  CV = _job_s_CV;
  while (true) {
    {
      lock_guard<mutex> lock(*M);
      CV->notify_all();
    }
    if (W->finished()) {
      break;
    }
    this_thread::sleep_for(_wait_for_ms);
  }
}

void WorkerManager::setWorkerInitialize(worker_init_t handler) {
  _worker_init_handler = handler;
}

string WorkerManager::workerName() { //
  return _name;
}

string WorkerManager::report() {
  string result = "[WorkerManager::report]\n";
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

void WorkerManager::addJob(const string &name,    //
                           job_handler_t handler, //
                           JOB_QUEUE_TYPE type,   //
                           bool affinity) {
  size_t job_id;
  size_t worker_id;
  shared_ptr<condition_variable> CV;
  shared_ptr<mutex> M;
  deque<shared_ptr<Job>> *Q;

  /// job_id 구하기 (MUST thread-safe)
  job_id = _job_seq.fetch_add(1);
  // printf("job_id: %ld\n", job_id);

  /// select worker
  /// TODO: job 분뱁 RR(Round Robin)
  if (affinity) {
    auto hashcode = hash<string>{}(name);
    worker_id = hashcode % _worker_num;
  } else {
    worker_id = job_id % _worker_num;
  }

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
    CV->notify_all();
  }
}

void WorkerManager::addJobSingleWorker(const string &name, //
                                       job_handler_t handler) {
  return addJob(name, handler, SINGLE_WORKER, false);
}

void WorkerManager::addJobMultiWorker(const string &name,    //
                                      job_handler_t handler, //
                                      bool affinity) {
  return addJob(name, handler, MULTI_WORKER, affinity);
}

void WorkerManager::run(bool block) {

  auto f = [&](shared_ptr<Worker> worker, //
               deque<shared_ptr<Job>> *Q, //
               shared_ptr<mutex> M,       //
               shared_ptr<condition_variable> CV) {
    shared_ptr<Job> job;
    const string worker_name = _name + "_" + job_type_str[worker->getJobType()];

    pthread_setname_np(pthread_self(), worker_name.c_str());

    /// worker initialize
    if (_worker_init_handler) {
      _worker_init_handler(worker);
    }

    while (true) {
      if (!_running && Q->empty()) {
        break;
      }
      {
        unique_lock<mutex> lock(*M.get());
        CV->wait_for(lock, _wait_for_ms);

        while (!Q->empty()) {
          job = Q->front();

          job->setWorkerID(worker->getWorkerID());
          auto handler = job->getHandler();
          handler(worker, job); // call job user defined function

          Q->pop_front();
          worker->incCompletedJobs(); // increment completed job count
        }
      }
    }

    /// worker thread 종료 되었음을 알림
    worker->terminate();
    pthread_exit(NULL);
    return;
  };

  _run(f, block);
}

void WorkerManager::_run(thread_handler_t f, bool block) {
  /// create job worker thread
  for (auto worker : _m_workers) {
    // thread create and run
    auto th = make_shared<thread>(thread(f,                                //
                                         worker,                           //
                                         &_job_m_Q[worker->getWorkerID()], //
                                         _job_m_M[worker->getWorkerID()],  //
                                         _job_m_CV[worker->getWorkerID()]));

    _worker_threads.push_back(th);
  }
  auto th = make_shared<thread>(thread(f,         //
                                       _s_worker, //
                                       &_job_s_Q, //
                                       _job_s_M,  //
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