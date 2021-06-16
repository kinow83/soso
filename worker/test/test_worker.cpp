
#include "worker.hpp"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;
using namespace soso;

int main(void) {
  WorkerManager manager(4, 100);

  manager.run(false);

  for (int i = 0; i < 4; i++) {
    manager.addJob(
        [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
          cout << "[multi] worker:" << worker->getWorkerID()
               << ", job :" << job->getJobID() << endl;
        },
        true);

    usleep(100);
  }
  for (int i = 0; i < 4; i++) {
    manager.addJob(
        [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
          cout << "[single] worker:" << worker->getWorkerID()
               << ", job :" << job->getJobID() << endl;
        },
        false);

    usleep(100);
  }

  sleep(2);
  manager.terminate(true);

  return 0;
}