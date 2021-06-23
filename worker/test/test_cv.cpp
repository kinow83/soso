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

condition_variable cv;
mutex m;
deque<int> q;
bool run = true;
bool ar = false;

void tfunc(int i) {
  // cout << "create thread# " << i << endl;

  unique_lock<mutex> lock(m);

  while (run) {
    // cout << "wait create thread# " << i << endl;
    // cv.wait_for(lock, 100ms);
    cv.wait(lock, []() { return !q.empty(); });
    while (true) {
      if (!ar) {
        cout << "wait" << endl;
        ar = true;
      }
      sleep(1);
    }

    // cout << "wakeup create thread# " << i << endl;

    if (!q.empty()) {
      int a = q.back();
      q.pop_back();
      cout << "\t[" << i << "] pop: " << a << " " << q.size() << "\n";
    }
  }
}

void notify(int i) {
  cout << "push: " << i << "\n";
  lock_guard<mutex> lock(m);
  q.push_front(i);
  cv.notify_all();
}

void notify2() {
  lock_guard<mutex> lock(m);
  cv.notify_one();
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  vector<thread> ts;

  for (int i = 0; i < 4; i++) {
    ts.push_back(thread(tfunc, i));
  }

  getchar();
  for (int i = 0; i < 10; i++) {
    notify(i);
  }

  cout << "q:" << q.size() << "\n";

  getchar();
  for (int i = 0; i < 10; i++) {
    notify(i);
  }

  sleep(100);
  run = false;

  return 0;
}