#include "pcapmon.h"
#include "echo.h"
#include "pcap_request.h"
#include <pcap.h>
#include <string>
#include <thread>

using namespace soso;
using namespace std;

static struct timeval tv = {.tv_sec = 10, .tv_usec = 0};

PcapMon::PcapMon(shared_ptr<ComponentChain> chain,     //
                 const string &source,                 //
                 size_t worker_num, int wait_for_ms) { //
  _chain = chain;
  _source = source;
  _worker_num = worker_num;
  _wait_for_ms = wait_for_ms;
  _worker_manager = make_shared<WorkerManager2>("pcap", _worker_num, 100);
  // run worker thread
  _worker_manager->run();
};

PcapMon::~PcapMon() {}

bool PcapMon::finished() { //
  return _finished;
}

void PcapMon::terminate() { //
  _running = false;
  _worker_manager->terminate();
  cout << _worker_manager->report() << endl;
  cout << _chain->report() << endl;
}

static void pcap_callback(u_char *args,                     //
                          const struct pcap_pkthdr *header, //
                          const u_char *packet) {
  static size_t req_idx = 0;
  auto pcapmon = reinterpret_cast<PcapMon *>(args);
  auto chain = pcapmon->getComponentChain();
  auto worker_manager = pcapmon->getWorkerManager();
  shared_ptr<PcapRequest> request =
      make_shared<PcapRequest>(req_idx++, header, packet);

  worker_manager->addJobMultiWorker( //
      "pcap wifi monitoring",        //
      [request, chain](shared_ptr<Worker> worker, shared_ptr<Job> job) {
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

void schedule_callback(int fd, short event, void *args) {
  auto pcapmon = reinterpret_cast<PcapMon *>(args);
  auto chain = pcapmon->getComponentChain();
  auto worker_manager = pcapmon->getWorkerManager();

  worker_manager->addJobSingleWorker( //
      "pcap wifi scheduler",          //
      [chain](shared_ptr<Worker> worker, shared_ptr<Job> job) {
        (void)worker;
        (void)job;
        chain->callSchedule();
      });

  evtimer_add(&pcapmon->getScheduleEvent(), &tv);
}

void PcapMon::run(bool block) {
  pcap_t *pcap;
  size_t captured = 0;

  event_init();

  pcap = create_pcap(_source, _wait_for_ms);
  if (!pcap) {
    goto end;
  }

  if (!_chain->initComponent()) {
    goto end;
  }

  evtimer_set(&_schedule_ev, schedule_callback, this);
  evtimer_add(&_schedule_ev, &tv);

  _running = true;
  while (_running) {
    event_loop(EVLOOP_NONBLOCK);

    captured = pcap_dispatch(pcap, 1, pcap_callback, (u_char *)this);
    if (captured == 0) {
      if (!block) {
        break;
      }
      usleep(_wait_for_ms);
    }
    _dispatched += captured;
  }

end:
  if (pcap)
    pcap_close(pcap);
  _finished = true;
  cout << "xxxx\n";
  return;
}