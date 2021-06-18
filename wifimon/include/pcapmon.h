#ifndef __SOSO_PCAP_MON_H__
#define __SOSO_PCAP_MON_H__

#include "component.h"
#include "worker.hpp"
#include <echo.h>
#include <memory>
#include <string>

namespace soso {
class PcapMon {
private:
  ComponentChain *_chain;
  std::string _source;
  size_t _worker_num;
  bool _running;

public:
  WorkerManager *_worker_manager;

public:
  PcapMon(ComponentChain *chain, //
          std::string source,    //
          size_t worker_num)
      : _chain(chain), _source(source), _worker_num(worker_num) { //
    _worker_manager = new WorkerManager(_worker_num, 100);
    _worker_manager->run();
  }

  ~PcapMon() {
    _worker_manager->terminate(true);
    delete _worker_manager;
  }

  WorkerManager *getWorkerManager() { //
    return _worker_manager;
  }

  ComponentChain *getComponentChain() { //
    return _chain;
  }

  void run(bool block);
};
}; // namespace soso

#endif