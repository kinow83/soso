#include "nlohmann/json.hpp"
#include "richevent_reader.h"
#include <iostream>

using namespace chkchk;
using namespace std;
using namespace nlohmann;

int main() {
  RichEventReader reader;
  reader.register_pull("test1", "tcp://127.0.0.1:5557",
                       [](zsock_t *zsock, reader_context_t &ctx) {
                         auto j = RichEventReader::recv_json(zsock, ctx);
                         if (!j.has_value()) {
                           return false;
                         }

                         std::cout
                             << "[pull] recv from port 5557: " << j->dump()
                             << std::endl;
                         return true;
                       });

  reader.register_sub("test2", "tcp://127.0.0.1:5558",
                      [](zsock_t *zsock, reader_context_t &ctx) {
                        auto j = RichEventReader::recv_json(zsock, ctx);
                        if (!j.has_value()) {
                          return false;
                        }

                        std::cout << "[pull] recv from port 5558: " << j->dump()
                                  << std::endl;
                        return true;
                      });

  reader.event_loop();
}