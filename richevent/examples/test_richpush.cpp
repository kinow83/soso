#include <czmq.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv) {
  zsys_handler_set(NULL);

  string endpoint = "tcp://127.0.0.1:" + string(argv[1]);
  zsock_t *writer = zsock_new_push(endpoint.c_str());

  for (int i = 0; i < 10; i++) {
    int rc = zstr_send(writer, string(endpoint + ":" + to_string(i)).c_str());
    cout << rc << endl;
    if (rc == -1) {
      cout << "send error\n";
      break;
    }
  }
  sleep(1);
  zsock_destroy(&writer);
  return 0;
}