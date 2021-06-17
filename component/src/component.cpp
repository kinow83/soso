#include "component.h"
#include "echo.h"
#include <unistd.h>

using namespace std;
using namespace soso;

ComponentChain::ComponentChain() {
  _chrono_stack.push_back(ChronoStack("init", 100));
  _chrono_stack.push_back(ChronoStack("prepare", 100));
  _chrono_stack.push_back(ChronoStack("process", 100));
  _chrono_stack.push_back(ChronoStack("post", 100));
  _chrono_stack.push_back(ChronoStack("schedule", 100));
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

bool ComponentChain::chronosCheckPoint(const string comp_name, int comp_idx,
                                       std::function<bool(void)> logic) {
  bool result;
  if (!_trace_chrono) {
    return logic();
  }

  Chrono chrono(comp_name);
  result = logic();
  chrono.end();

  _chrono_lock.lock();
  _chrono_stack[comp_idx].addChrono(chrono);
  _chrono_lock.unlock();

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
      return ok;
    }
  }
  return ok;
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
      return;
    }
  }
  return;
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
      return;
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
      return;
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
      return;
    }
  }
}

void ComponentChain::showComponent() {
  echo.t("[module chains]-------\n");
  for (auto c : _chains) {
    echo.t("[%s]\n", c->getName());
  }
}
