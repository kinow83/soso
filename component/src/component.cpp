#include "component.h"
#include <unistd.h>

using namespace std;
using namespace soso;

ComponentChain::ComponentChain() {
  size_t max_point = _chains.size();
  _kstack.push_back(KairosStack("init", max_point));
  _kstack.push_back(KairosStack("prepare", max_point));
  _kstack.push_back(KairosStack("process", max_point));
  _kstack.push_back(KairosStack("post", max_point));
  _kstack.push_back(KairosStack("schedule", max_point));
}

ComponentChain::~ComponentChain() { //
}

void ComponentChain::registers(shared_ptr<Component> component) {
  for (auto c : _chains) {
    if (c->getName() == component->getName()) {
      throw runtime_error("already registered component: " + c->getName());
    }
  }
  _chains.push_back(component);
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
  auto it = _chains.begin();
  while (it != _chains.end()) {
    if ((*it)->getName() == name) {
      _chains.erase(it);
      removed++;
    } else {
      ++it;
    }
  }
  if (removed == 0) {
    throw runtime_error("no such component: " + name);
  }
}

void ComponentChain::kairosMonitor(COMPONENT_IDX comp_idx,
                                   function<void(KairosStack &)> f) {
  _kstack[comp_idx].monitoring(f);
}

bool ComponentChain::kairosCheck(const string comp_name, int comp_idx,
                                 function<bool(void)> logic) {
  if (!_trace_kairos) {
    return logic();
  }

  {
    Kairos karios(comp_name, &_kstack[comp_idx]);
    return logic();
  }
}

bool ComponentChain::initComponent() {
  bool ok;
  for (auto c : _chains) {
    ok = kairosCheck(c->getName(),   //
                     COMPONENT_INIT, //
                     [c](void) { return c->init(); });
    if (!ok) {
      return ok;
    }
  }
  return ok;
}

void ComponentChain::callSchedule(void) {
  bool ok;
  for (auto c : _chains) {
    ok = kairosCheck(c->getName(),       //
                     COMPONENT_SCHEDULE, //
                     [c](void) {
                       c->schedule();
                       return true;
                     });
    if (!ok) {
      return;
    }
  }
  return;
}

void ComponentChain::callComponent(Request *request) {
  bool ok;
  if (request == nullptr)
    return;

  // prepare
  for (auto c : _chains) {
    ok = kairosCheck(c->getName(),        //
                     COMPONENT_PREPARE,   //
                     [c, request](void) { //
                       return c->prepare(request);
                     });
    if (!ok) {
      return;
    }
  }

  // process
  for (auto c : _chains) {
    ok = kairosCheck(c->getName(),        //
                     COMPONENT_PROCESS,   //
                     [c, request](void) { //
                       return c->process(request);
                     });
    if (!ok) {
      return;
    }
  }

  // post
  for (auto c : _chains) {
    ok = kairosCheck(c->getName(),        //
                     COMPONENT_POST,      //
                     [c, request](void) { //
                       return c->post(request);
                     });
    if (!ok) {
      return;
    }
  }
}
