#ifndef __SOSO_COMPONENT_SESSION_H__
#define __SOSO_COMPONENT_SESSION_H__

#include "component.h"
#include "hash.hpp"
#include "pcap_request.h"
#include "restapi.h"

namespace soso {

class ComponentSession : public Component {
private:
  std::shared_ptr<KHash<ap_info_t>> apDB;
  std::shared_ptr<KHash<st_info_t>> stDB;
  std::shared_ptr<RestApi> api;

public:
  void showAP();
  void showST();

public:
  ComponentSession(const std::string &name) : Component(name) {}
  virtual ~ComponentSession();
  virtual bool init();
  virtual bool prepare(Request *request);
  virtual bool process(Request *request);
  virtual bool post(Request *request);
  virtual void schedule();
};
}; // namespace soso

#endif