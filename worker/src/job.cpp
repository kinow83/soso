

#include "worker.h"

using namespace std;
using namespace soso;

Job::Job(const std::string &name, job_handler_t &handler, bool affinity) { //
  _name = name;
  _handler = handler;
  _hashcode = std::hash<std::string>{}(name);
  _affinity = affinity;
}

bool Job::affinity() { //
  return _affinity;
}

size_t Job::jobHashCode() { //
  return _hashcode;
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