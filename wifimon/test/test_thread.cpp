#include <pthread.h>
#include <thread>
#include <unistd.h>

using namespace std;

class Bar {
private:
  void foo(int i) {
    pthread_setname_np(pthread_self(), "kaka");
    while (1) {
      sleep(1);
    }
  }

public:
  void run() { //
    std::thread t1(&Bar::foo, this, 1);
    t1.join();
  }
};

int main(void) {
  Bar b;
  b.run();
}