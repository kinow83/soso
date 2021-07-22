#include "richevent.h"
#include <iostream>

int main() {
  RichEventBase rbase;

  rbase.subscribe("tcp://127.0.0.1:5557", //
                  [](zsock_t *zsock, RichEventBase *rbase) {
                    (void)rbase;
                    char *msg = zstr_recv(zsock);
                    if (!msg)
                      return false;

                    std::cout << "recv from port 5557: " << msg << std::endl;
                    zstr_free(&msg);
                    return true;
                  });

  rbase.subscribe("tcp://127.0.0.1:5558", //
                  [](zsock_t *zsock, RichEventBase *rbase) {
                    (void)rbase;
                    char *msg = zstr_recv(zsock);
                    if (!msg)
                      return false;

                    std::cout << "recv from port 5558: " << msg << std::endl;
                    zstr_free(&msg);
                    return true;
                  });

  rbase.event_loop();
}