
#include "worker.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace soso;

int main(int argc, char **argv) {
  WorkerManager *manager;

  int th_num = atoi(argv[1]);

#if 1
  {
    bool affinity = true;
    manager = new WorkerManager("test", th_num, 100);
    manager->run(false);

    /// 여러개의 worker가 job을 처리함
    cout << "[multi worker] =================\n";
    for (int i = 0; i < 8; i++) {
      manager->addJobMultiWorker(
          "test1", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            (void)(worker);
            (void)(job);
            this_thread::sleep_for(1s);
            /*
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
                 */
          },
          affinity);

      cout << "\tpush #" + to_string(i + 1) << endl;
    }
    manager->terminate();
    cout << manager->report() << endl;

    delete manager;
  }
#endif

#if 1
  {
    manager = new WorkerManager("test", th_num, 100);
    manager->run(false);

    /// 하나의 지정된 worker가 job을 처리함
    /// 지정된 worker는 job name의 문자열 해쉬값으로 worker가 선택됨
    cout << "[multi worker with affinity #1] =================\n";
    for (int i = 0; i < 8; i++) {
      manager->addJobMultiWorker(
          "test2", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            (void)(worker);
            (void)(job);
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          },
          true);
      cout << "\tpush #" + to_string(i + 1) << endl;
    }
    manager->terminate();
    cout << manager->report() << endl;
    delete manager;
  }
  {
    manager = new WorkerManager("test", th_num, 100);
    manager->run(false);

    cout << "[multi worker with affinity #2] =================\n";
    for (int i = 0; i < 8; i++) {
      manager->addJobMultiWorker(
          "testtest2", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            (void)(worker);
            (void)(job);
            cout << "[multi] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
          },
          true);
      cout << "\tpush #" + to_string(i + 1) << endl;
    }
    manager->terminate();
    cout << manager->report() << endl;
    delete manager;
  }
#endif

#if 1
  {
    manager = new WorkerManager("test", th_num, 100);
    manager->run(false);

    /// 단일 worker가 job을 처리함
    cout << "[single worker] =================\n";
    for (int i = 0; i < 8; i++) {
      manager->addJobSingleWorker(
          "test3", //
          [](std::shared_ptr<Worker> worker, std::shared_ptr<Job> job) {
            (void)(worker);
            (void)(job);
            this_thread::sleep_for(1s);
            /*
            cout << "[single] worker:" << worker->getWorkerID()
                 << ", job :" << job->getJobID() << endl;
                 */
          });
      cout << "\tpush #" + to_string(i + 1) << endl;
    }
    manager->terminate();
    cout << manager->report() << endl;
    delete manager;
  }
#endif
  return 0;
}