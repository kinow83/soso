#include "worker.h"
#include <pthread.h>

using namespace std;
using namespace soso;

static const char *job_type_str[] = { //
    "m_worker",                       //
    "s_worker"};

void WorkerManager2::run(bool block) {

  auto f = [&](shared_ptr<Worker> worker, //
               deque<shared_ptr<Job>> *Q, //
               shared_ptr<mutex> M,       //
               shared_ptr<condition_variable> CV) {
    shared_ptr<Job> job;
    job_handler_t handler;
    const string worker_name = _name + "_" + job_type_str[worker->getJobType()];

    pthread_setname_np(pthread_self(), worker_name.c_str());

    /// worker initialize
    if (_worker_init_handler) {
      _worker_init_handler(worker);
    }

    while (true) {
      unique_lock<mutex> lock(*M);
      CV->wait(lock,   //
               [&]() { //
                 return (!Q->empty() || !_running);
               });

      if (!_running && Q->empty()) {
        break;
      }

      handler = nullptr;
      if (!Q->empty()) {
        job = Q->front();
        job->setWorkerID(worker->getWorkerID());
        handler = job->getHandler();
        Q->pop_front();

        /// increment completed job count
        worker->incCompletedJobs();
      }
      if (handler != nullptr)
        handler(worker, job); // call job user defined function
    }

    /// worker thread 종료 되었음을 알림
    worker->terminate();
    pthread_exit(NULL);
    return;
  };

  _run(f, block);
}
