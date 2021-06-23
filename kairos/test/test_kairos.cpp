#include "kairos.h"
#include <gtest/gtest.h>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace soso;

TEST(kairos_test, normal) {
  KairosStack kstack("Test", 100);
  vector<Kairos> kairos;

  for (int i = 0; i < 10; i++) {
    kairos.push_back(Kairos("kairos#" + to_string(i)));
  }

  for (auto &kairo : kairos) {
    kairo.begin();
    usleep(10);
    kairo.end();
    kstack.addKairos(kairo);
  }
  cout << kstack.toString();
}

TEST(kairos_test, limit_stack_size) {
  KairosStack kstack("Test", 4);
  vector<Kairos> kairos;

  for (int i = 0; i < 10; i++) {
    kairos.push_back(Kairos("kairos#" + to_string(i)));
  }

  for (auto &kairo : kairos) {
    kairo.begin();
    usleep(10);
    kairo.end();
    kstack.addKairos(kairo);
  }
  cout << kstack.toString();
}

TEST(kairos_test, rvalue) {
  KairosStack kstack("Test", 10);
  vector<Kairos> kairos;

  for (int i = 0; i < 10; i++) {
    Kairos kairos("kairos#" + to_string(i), &kstack);
    usleep(10);
  }
  cout << kstack.toString();
}
