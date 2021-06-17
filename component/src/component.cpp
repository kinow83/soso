#include "component.h"
#include "echo.h"
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;
using namespace soso;

ComponentChain::ComponentChain() {
  for (auto c : _chronos) {
    c.setMaxCheckPoint(1000);
    c.setMaxLongTimeCheckPoint(500);
  }
}

void ComponentChain::registers(shared_ptr<Component> component) {
  for (auto c : _chains) {
    if (c->getName() == component->getName()) {
      throw runtime_error("already registered component: " + c->getName());
    }
  }
  this->_chains.push_back(component);
}

shared_ptr<Component> ComponentChain::operator[](const string name) {
  for (auto c : _chains) {
    if (c->getName() == name) {
      return c;
    }
  }
  throw runtime_error("already registered component: " + name);
}

void ComponentChain::remove(const string name) {
  int removed = 0;
  auto iter = _chains.begin();
  while (iter != _chains.end()) {
    if ((*iter)->getName() == name) {
      _chains.erase(iter);
      removed++;
    } else {
      ++iter;
    }
  }
  if (removed == 0) {
    throw runtime_error("no such component: " + name);
  }
}

bool ComponentChain::chronosCheckPoint(const string &comp_name, int comp_idx,
                                       std::function<bool(void)> logic) {

  bool result;
  if (!_trace_chronos) {
    return logic();
  }

  const lock_guard<mutex> lock(_chronos_lock);
  _chronos[comp_idx].checkpoint(comp_name);

  result = logic();

  _chronos[comp_idx].checkpoint();
  return result;
}

bool ComponentChain::initComponent() {
  bool ok;
  for (auto c : _chains) {
    ok = chronosCheckPoint(c->getName(),   //
                           COMPONENT_INIT, //
                           [&c](void) { return c->init(); });
    if (!ok) {
      echo.e("Fail init for '%s' component", c->getName());
    }
  }
}

void ComponentChain::callSchedule(void) {
  bool ok;
  for (auto c : _chains) {
    ok = chronosCheckPoint(c->getName(),       //
                           COMPONENT_SCHEDULE, //
                           [&c](void) {
                             c->schedule();
                             return true;
                           });
    if (!ok) {
      echo.e("Fail schedule for '%s' component", c->getName());
    }
  }
}

void ComponentChain::callComponent(shared_ptr<Request> request) {
  bool ok;
  if (request == nullptr)
    return;

  // prepare
  for (auto c : _chains) {
    ok = chronosCheckPoint(c->getName(),          //
                           COMPONENT_PREPARE,     //
                           [&c, &request](void) { //
                             return c->prepare(request);
                           });
    if (!ok) {
      echo.e("Fail prepare for '%s' component", c->getName());
    }
  }

  // process
  for (auto c : _chains) {
    ok = chronosCheckPoint(c->getName(),          //
                           COMPONENT_PROCESS,     //
                           [&c, &request](void) { //
                             return c->process(request);
                           });
    if (!ok) {
      echo.e("Fail process for '%s' component", c->getName());
    }
  }

  // post
  for (auto c : _chains) {
    ok = chronosCheckPoint(c->getName(),          //
                           COMPONENT_POST,        //
                           [&c, &request](void) { //
                             return c->post(request);
                           });
    if (!ok) {
      echo.e("Fail post for '%s' component", c->getName());
    }
  }
}

void ComponentChain::showComponent() {
  echo.t("[module chains]-------\n");
  for (auto c : _chains) {
    echo.t("[%s]\n", c->getName());
  }
}
