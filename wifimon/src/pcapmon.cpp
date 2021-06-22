#include "pcapmon.h"
#include "echo.h"
#include "pcap_request.h"
#include <pcap.h>
#include <string>

using namespace soso;
using namespace std;

PcapMon::PcapMon(std::shared_ptr<ComponentChain> chain, //
                 const std::string &source,             //
                 size_t worker_num, int wait_ms) {      //
  _chain = chain;
  _source = source;
  _worker_num = worker_num;
  _wait_ms = wait_ms;
  _worker_manager = std::make_shared<WorkerManager>("pcap", _worker_num, 100);
  // run worker thread
  _worker_manager->run();
};

PcapMon::~PcapMon() {
  _worker_manager->terminate(true);
  std::cout << _worker_manager->report() << std::endl;
}

static void pcap_callback(u_char *args,                     //
                          const struct pcap_pkthdr *header, //
                          const u_char *packet) {
  static size_t req_idx = 0;
  PcapMon *pcapmon = reinterpret_cast<PcapMon *>(args);
  auto chain = pcapmon->getComponentChain();
  auto worker_manager = pcapmon->getWorkerManager();
  shared_ptr<PcapRequest> request =
      make_shared<PcapRequest>(req_idx++, header, packet);

  worker_manager->addJobMultiWorker( //
      "pcap wifi monitoring",        //
      [request, chain](std::shared_ptr<Worker> worker,
                       std::shared_ptr<Job> job) {
        (void)worker;
        (void)job;
        chain->callComponent(request.get());
      });
}

static pcap_t *create_pcap(const string &source, int wait_ms) {
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *pcap;
  int promisc = 1;

  if (strstr(source.c_str(), ".pcap")) { // .pcap, .pcapng
    pcap = pcap_open_offline(source.c_str(), errbuf);
  } else {
    pcap = pcap_open_live(source.c_str(), BUFSIZ, promisc, wait_ms, errbuf);
  }
  if (!pcap) {
    echo.F("pcap create fail: %s", errbuf);
  }
  pcap_setnonblock(pcap, 1, errbuf);
  return pcap;
}

void PcapMon::run(bool block) {
  pcap_t *pcap;
  size_t captured = 0;

  pcap = create_pcap(_source, _wait_ms);
  if (!pcap) {
    goto end;
  }

  if (!_chain->initComponent()) {
    goto end;
  }

  _running = true;
  while (_running) {
    captured = pcap_dispatch(pcap, 1, pcap_callback, (u_char *)this);

    if (captured == 0) {
      if (!block) {
        break;
      }
      usleep(_wait_ms);
    }
    _dispatched += captured;
  }

end:
  if (pcap)
    pcap_close(pcap);
  return;
}