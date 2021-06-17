#include "hash.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace std;
using namespace soso;

static int thread_on_ready = 0;
static int thread_terminate = 0;

__thread int node_num_1;
__thread int node_num_2;

typedef struct unittest_t {
  int value;
  char name[64];
} unittest_t;

Hash<unittest_t *> *ht;

long hash_func(unittest_t *&t) { //
  return t->value;
}

int hash_compare(unittest_t *&t1, unittest_t *&t2) { //
  return t1->value - t2->value;
}

void data_free(unittest_t *&t) { //
  free(t);
}

std::optional<unittest_t *> data_copy(unittest_t *&t) {
  unittest_t *a = (unittest_t *)malloc(sizeof(unittest_t));
  memcpy(a, t, sizeof(unittest_t));
  return make_optional(a);
}

std::string to_string(unittest_t *&t) {
  //
  return string(t->name);
}

void *thread_func(void *arg) {
  int i = reinterpret_cast<int>(arg);
  int cnt = 0;

  while (thread_on_ready == 0) {
    usleep(1);
  }

  node_num_1 = (i * 1000) + (cnt);
  while (thread_terminate == 0) {
    unittest_t *t = (unittest_t *)malloc(sizeof(unittest_t));
    t->value = (i * 1000) + (cnt++);
    snprintf(t->name, sizeof(t->name), "n#%d", t->value);

    assert(ht->insert(t) == true);
    usleep(1);
  }
  node_num_2 = (i * 1000) + (cnt);

  printf("[%d] %d ~ %d\n", i, node_num_1, node_num_2);

  while (node_num_2 != node_num_1) {
    unittest_t t, *pt;
    t.value = node_num_1;
    pt = &t;
    assert(ht->exist(pt) == true);
    node_num_1++;
  }

  int total = 0;
  ht->loop([&total](unittest_t *data) { //
    total += data->value;
  });

  return NULL;
}

int main(int argc, char **argv) {
  const int plen = 5;
  pthread_t pid[plen];
  argc = argc;
  argv = argv;

  ht = new Hash<unittest_t *>(10,           //
                              hash_func,    //
                              hash_compare, //
                              data_free,    //
                              data_copy,    //
                              to_string,    //
                              false);

  for (int i = 0; i < plen; i++) {
    pthread_create(&pid[i], NULL, thread_func, (void *)(i + 1));
  }

  sleep(1);
  thread_on_ready = 1;

  usleep(5000);

  thread_terminate = 1;

  for (int i = 0; i < plen; i++) {
    pthread_join(pid[i], NULL);
  }

  sleep(1);

  // ht->show(NULL);
  delete ht;

  printf("success\n");
  return 0;
}