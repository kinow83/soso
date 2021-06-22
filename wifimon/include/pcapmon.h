#ifndef __SOSO_PCAP_MON_H__
#define __SOSO_PCAP_MON_H__

#include "component.h"
#include "worker.h"
#include <echo.h>
#include <memory>
#include <string>
#include <thread>

namespace soso {
class PcapMon {
private:
  std::shared_ptr<ComponentChain> _chain;
  std::shared_ptr<WorkerManager> _worker_manager;
  std::string _source;
  size_t _worker_num;
  bool _running;
  size_t _dispatched;
  int _wait_ms;
  std::thread _schedule_thread;

public:
  PcapMon(std::shared_ptr<ComponentChain> chain, //
          const std::string &source,             //
          size_t worker_num, int wait_ms);

  ~PcapMon();

  std::shared_ptr<WorkerManager> getWorkerManager() { //
    return _worker_manager;
  }

  std::shared_ptr<ComponentChain> getComponentChain() { //
    return _chain;
  }

  void run(bool block);
};
}; // namespace soso

#endif