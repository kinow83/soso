
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
  WorkerManager *manager;

  {
    manager = new WorkerManager(8, 100);
    manager->run(false);

    /// 여러개의 worker가 job을 처리함
    cout << "[multi worker] =================\n";
    for (int i = 0; i < 16; i++) {
      manager->addJobMultiWorker(
          "test1", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          });
    }
    manager->terminate(true);
    delete manager;
  }

  {
    manager = new WorkerManager(8, 100);
    manager->run(false);

    /// 하나의 지정된 worker가 job을 처리함
    /// 지정된 worker는 job name의 문자열 해쉬값으로 worker가 선택됨
    cout << "[multi worker with affinity #1] =================\n";
    for (int i = 0; i < 4; i++) {
      manager->addJobMultiWorker(
          "test2", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          },
          true);
    }
    manager->terminate(true);
    delete manager;
  }
  {
    manager = new WorkerManager(8, 100);
    manager->run(false);

    cout << "[multi worker with affinity #2] =================\n";
    for (int i = 0; i < 4; i++) {
      manager->addJobMultiWorker(
          "testtest2", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          },
          true);
    }
    manager->terminate(true);
    delete manager;
  }

  {
    manager = new WorkerManager(8, 100);
    manager->run(false);

    /// 단일 worker가 job을 처리함
    cout << "[single worker] =================\n";
    for (int i = 0; i < 4; i++) {
      manager->addJobSingleWorker(
          "test3", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            cout << "[single] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          });
    }
    manager->terminate(true);
    delete manager;
  }

  return 0;
}