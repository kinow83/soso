

#include "worker.h"

using namespace std;
using namespace soso;

Job::Job(size_t job_id, job_handler_t &handler,
         bool affinity) { //
  _job_id = job_id;
  _handler = handler;
  _affinity = affinity;
}

bool Job::affinity() { //
  return _affinity;
}

job_handler_t Job::getHandler() { //
  return _handler;
}

void Job::setWorkerID(std::size_t worker_id) { //
  _worker_id = worker_id;
}

void Job::setJobID(std::size_t job_id) { //
  _job_id = job_id;
}

size_t Job::getWorkerID() const { //
  return _worker_id;
}

size_t Job::getJobID() const { //
  return _job_id;
}