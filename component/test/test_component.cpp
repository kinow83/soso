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
    usleep(100);
    return noop();
  }
  bool prepare(std::shared_ptr<Request> request) { //
    request = request;
    usleep(100);
    return noop();
  }
  bool process(std::shared_ptr<Request> request) { //
    request = request;
    usleep(100);
    return noop();
  }
  bool post(std::shared_ptr<Request> request) { //
    request = request;
    usleep(100);
    return noop();
  }
  void schedule() { //
    usleep(100);
    return;
  }
};

int main(void) { //

  ComponentChain chain;

  chain.registers(make_shared<ComponentTest>(ComponentTest("test#1")));
  chain.registers(make_shared<ComponentTest>(ComponentTest("test#2")));
  chain.registers(make_shared<ComponentTest>(ComponentTest("test#3")));
  chain.registers(make_shared<ComponentTest>(ComponentTest("test#4")));
  chain.registers(make_shared<ComponentTest>(ComponentTest("test#5")));

  chain.initComponent();
  chain.callComponent(make_shared<Request>(Request()));
  chain.callSchedule();

  cout << chain.getChronoResult() << endl;
  cout << chain.componentSize() << endl;

  chain.chronosMonitoring( //
      COMPONENT_PROCESS,   //
      [](ChronosStack &stack) {
        cout << "monitoring: " << stack.title() << endl;
        if (stack.getTotal().tv_usec > 10) {
          cout << "BAD process" << endl;
        } else {
          cout << "GOOD process" << endl;
        }
      });

  return 0;
}
