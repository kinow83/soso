#include "worker.h"
#include <pthread.h>

using namespace std;
using namespace soso;

static const char *job_type_str[] = { //
    "m_worker",                       //
    "s_worker"};

void WorkerManager2::run(bool block) { //
  _run(&WorkerManager2::workerThreadFunc, block);
}

/**
 * @brief ModuleWorker 생성자
 * @param worker_num worker(작업자) 수
 * @param wait_for_ms job(작업) 용처 대기 시간 (millisecond)
 */
WorkerManager2::WorkerManager2(const std::string &name, int worker_num)
    : WorkerManager(name, worker_num, 10) {
  //
}

void WorkerManager2::workerThreadFunc(shared_ptr<Worker> worker,
                                      deque<shared_ptr<Job>> *Q,
                                      shared_ptr<mutex> M,
                                      shared_ptr<condition_variable> CV) {
  shared_ptr<Job> job;
  const string worker_name = _name + "_" + job_type_str[worker->getJobType()];
  job_handler_t handler;

  pthread_setname_np(pthread_self(), worker_name.c_str());

  /// worker initialize
  if (_worker_init_handler) {
    _worker_init_handler(worker);
    cout << "---------------\n";
  }

  while (true) {
    {
      unique_lock<mutex> lock(*M.get());
      // wait for job wakeup signal
      CV->wait(lock,
               [&]() { //
                 return (!Q->empty() || !_running);
               });

      if (!_running && Q->empty()) {
        break;
      }

      job = Q->front();
      job->setWorkerID(worker->getWorkerID());
      handler = job->getHandler();
      Q->pop_front();

      /// increment completed job count
      worker->incCompletedJobs();
    }

    handler(worker, job); // call job user defined function
  }

  /// worker thread 종료 되었음을 알림
  worker->terminate();
  pthread_exit(NULL);
  return;
}
