#include "component.h"
#include <iostream>
#include <unistd.h>

using namespace soso;
using namespace std;

class ComponentTest : public Component {
public:
  ComponentTest(std::string name) : Component(name) {}
  ~ComponentTest() {}
  bool init() { //
    usleep(10);
    return noop();
  }
  bool prepare(Request *request) { //
    request = request;
    usleep(10);
    return noop();
  }
  bool process(Request *request) { //
    request = request;
    usleep(10);
    return noop();
  }
  bool post(Request *request) { //
    request = request;
    usleep(10);
    return noop();
  }
  void schedule() { //
    usleep(10);
    return;
  }
};

int main(void) { //

  ComponentChain chain;
  Request *request = new Request();

  chain.registers(make_shared<ComponentTest>("test#1"));
  chain.registers(make_shared<ComponentTest>("test#2"));
  chain.registers(make_shared<ComponentTest>("test#3"));
  chain.registers(make_shared<ComponentTest>("test#4"));
  chain.registers(make_shared<ComponentTest>("test#5"));

  chain.initComponent();
  chain.callComponent(request);
  chain.callSchedule();

  cout << chain.report() << endl;
  cout << chain.componentSize() << endl;

  chain.kairosMonitor(   //
      COMPONENT_PROCESS, //
      [](KairosStack &stack) {
        cout << "monitoring: " << stack.title() << endl;
        if (stack.getTotal().tv_usec > 10) {
          cout << "BAD process" << endl;
        } else {
          cout << "GOOD process" << endl;
        }
      });

  return 0;
}
