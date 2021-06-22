
#include "chronos.h"
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

int main(int argc, char **argv) {
  argc = argc;

  ChronosStack cstack("test_mysql", 1000);

  int thread_num = atoi(argv[1]);

  shared_ptr<WorkerManager> manager =
      make_shared<WorkerManager>("mysql", thread_num, 1000);

  auto driver = get_driver_instance();

  manager->setWorkerInitialize([&driver](shared_ptr<Worker> worker) { //
    try {
      shared_ptr<Connection> conn(
          driver->connect("tcp://127.0.0.1:3306", "api", "qwer1234"));
      conn->setSchema("api");
      cout << "create sql connection: #" << worker->getWorkerID() << endl;

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

  Chronos c("run", &cstack);
  {
    {
      Chronos c("add job", &cstack);
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
              res = stmt->executeQuery("SELECT * FROM foo");
              while (res->next()) {
#if 0
            cout << res->getString("id") << ":" << res->getString("name")
                 << endl;
#endif
              }
              cout << "ok\n";
            });
        sleep(1);
        cout << "push\n";
      }
    }

    manager->terminate(true);
  }
  cout << cstack.toString();
#if 0
  cout << "checkedCount: " << cstack.checkedCount() << endl;
  cout << "pointSize: " << cstack.pointSize() << endl;
#endif
}