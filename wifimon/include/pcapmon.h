#ifndef __SOSO_PCAP_MON_H__
#define __SOSO_PCAP_MON_H__

#include "component.h"
#include "worker.h"
#include <echo.h>
#include <event.h>
#include <memory>
#include <string>
#include <thread>

namespace soso {
class PcapMon {
private:
  std::shared_ptr<ComponentChain> _chain;
  std::shared_ptr<WorkerManager2> _worker_manager;
  std::string _source;
  size_t _worker_num;
  bool _running;
  bool _finished = false;
  size_t _dispatched;
  int _wait_for_ms;
  struct event _schedule_ev;

public:
  PcapMon(std::shared_ptr<ComponentChain> chain, //
          const std::string &source,             //
          size_t worker_num, int wait_ms);

  ~PcapMon();

  std::shared_ptr<WorkerManager2> getWorkerManager() { return _worker_manager; }
  std::shared_ptr<ComponentChain> getComponentChain() { return _chain; }
  struct event &getScheduleEvent() {
    return _schedule_ev;
  }
  bool finished();
  void terminate();
  void run(bool block);
};
}; // namespace soso

#endif