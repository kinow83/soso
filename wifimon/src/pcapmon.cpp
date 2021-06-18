#include "pcapmon.h"
#include "echo.h"
#include "pcap_request.h"
#include <pcap.h>
#include <string>

using namespace soso;
using namespace std;

static void pcap_callback(u_char *args,                     //
                          const struct pcap_pkthdr *header, //
                          const u_char *packet) {
  PcapMon *pcapmon = reinterpret_cast<PcapMon *>(args);
  ComponentChain *chain = pcapmon->getComponentChain();
  WorkerManager *worker_manager = pcapmon->getWorkerManager();
  PcapRequest request(header, packet);

  worker_manager->addJobMultiWorker( //
      "pcap wifi monitoring",        //
      [&request, chain](std::shared_ptr<Worker> worker,
                        std::shared_ptr<Job> job) {
        (void)(worker);
        (void)(job);
        chain->callComponent(&request);
      });
}

static pcap_t *create_pcap(const string &source) {
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *pcap;
  int promisc = 1;
  int to_ms = 10;

  if (strstr(source.c_str(), ".pcap")) { // .pcap, .pcapng
    pcap = pcap_open_offline(source.c_str(), errbuf);
  } else {
    pcap = pcap_open_live(source.c_str(), BUFSIZ, promisc, to_ms, errbuf);
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

  pcap = create_pcap(_source);
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
      usleep(10);
    }
  }

end:
  if (pcap)
    pcap_close(pcap);
  return;
}