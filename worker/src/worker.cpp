#include "worker.h"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdint.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace soso;

Worker::Worker(WorkerManager *worker_manager, //
               int worker_id,                 //
               JOB_QUEUE_TYPE job_type) {

  _worker_manager = worker_manager;
  _worker_id = worker_id;
  _job_type = job_type;
  _completed_jobs = 0;
}

shared_ptr<void> Worker::getCTX() { //
  return _ctx;
}

void Worker::setCTX(shared_ptr<void> ctx) { //
  _ctx = ctx;
}

JOB_QUEUE_TYPE Worker::getJobType() { //
  return _job_type;
}

const WorkerManager *Worker::getWorkerManager() { //
  return _worker_manager;
}

bool Worker::finished() { //
  return (_running == false);
}

void Worker::terminate() { //
  _running = false;
}

std::size_t Worker::getWorkerID() { //
  return _worker_id;
}

std::size_t Worker::getCompletedJobs() { //
  return _completed_jobs;
}

std::size_t Worker::incCompletedJobs() { //
  return ++_completed_jobs;
}