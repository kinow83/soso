#include "nlohmann/json.hpp"
#include "richevent_writer.h"
#include <iostream>

using namespace chkchk;

int main() {
  RichEventWriter writer;
  writer.register_push("test1", "tcp://127.0.0.1:5557");
  writer.register_pub("test2", "tcp://127.0.0.1:5558");

  int cnt = 0;
  while (true) {
    // writer.publish("test1", "push test1 aaaaaaaaaaaaaaaa");
    nlohmann::json j;
    j["id"] = cnt;
    j["message"] = "aaaaaaaaaaaaaaa";
    writer.send_json("test1", j);

    j["id"] = cnt++;
    j["message"] = "bbbbbbbbbbbbbbb";
    writer.send_json("test2", j);

    sleep(1);
  }
}