#include "chronos.h"
#include <gtest/gtest.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace soso;

TEST(chronos_test, normal) {
  ChronosStack cstack("Test", 100);
  vector<Chronos> chronos;

  for (int i = 0; i < 10; i++) {
    chronos.push_back(Chronos("chronos#" + to_string(i)));
  }

  for (auto &chrono : chronos) {
    chrono.begin();
    usleep(10);
    chrono.end();
    cstack.addChronos(chrono);
  }
  cout << cstack.toString();
}

TEST(chronos_test, limit_stack_size) {
  ChronosStack cstack("Test", 4);
  vector<Chronos> chronos;

  for (int i = 0; i < 10; i++) {
    chronos.push_back(Chronos("chronos#" + to_string(i)));
  }

  for (auto &chrono : chronos) {
    chrono.begin();
    usleep(10);
    chrono.end();
    cstack.addChronos(chrono);
  }
  cout << cstack.toString();
}

TEST(chronos_test, rvalue) {
  ChronosStack cstack("Test", 10);
  vector<Chronos> chronos;

  for (int i = 0; i < 10; i++) {
    Chronos chrons("chronos#" + to_string(i), &cstack);
    usleep(10);
  }
  cout << cstack.toString();
}
