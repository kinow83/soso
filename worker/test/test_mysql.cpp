
#include "kairos.h"
#include "worker.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std;
using namespace soso;
using namespace sql;

template <typename T> //
void mysql_test(int thread_num) {
  KairosStack cstack("test_mysql", 1000);

  shared_ptr<T> manager = make_shared<T>("mysql", thread_num);

  auto driver = get_driver_instance();

  manager->setWorkerInitialize([&driver](shared_ptr<Worker> worker) { //
    try {
      shared_ptr<Connection> conn(
          driver->connect("tcp://127.0.0.1:3306", "api", "qwer1234"));
      conn->setSchema("api");
      // cout << "create sql connection: #" << worker->getWorkerID() << endl;

      worker->setCTX(conn);
    } catch (sql::SQLException &e) {
      cout << "# ERR: SQLException in " << __FILE__;
      cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
      cout << "# ERR: " << e.what();
      cout << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }
  });

  manager->run(false);
  sleep(3);
  cout << "wait for thread init\n";

  {
    Kairos c("run", &cstack);
    {
      Kairos c("add job", &cstack);
      for (int i = 0; i < 4; i++) {
        manager->addJobMultiWorker(
            "select",                                            //
            [](shared_ptr<Worker> worker, shared_ptr<Job> job) { //
              sql::Statement *stmt;
              sql::ResultSet *res;
              (void)job;
              sleep(5);
              shared_ptr<void> con = worker->getCTX();
              Connection *conn = reinterpret_cast<Connection *>(con.get());
              stmt = conn->createStatement();
              res = stmt->executeQuery("SELECT id, name FROM foo");
              while (res->next()) {
#if 0
            cout << res->getString("id") << ":" << res->getString("name")
                 << endl;
#endif
              }
              cout << "\tpop #" << worker->getWorkerID() << endl;
            });
        sleep(1);
        cout << "push\n";
      }
    }

    manager->terminate();
  }
  cout << cstack.toString();
}

int main(int argc, char **argv) {
  argc = argc;
  int thread_num = atoi(argv[1]);

  cout << "WorkerManager ================================\n";
  mysql_test<WorkerManager>(thread_num);

  cout << "\n\n\n";
  cout << "WorkerManager2 ================================\n";
  mysql_test<WorkerManager2>(thread_num);
}
