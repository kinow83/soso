
#include "component80211.h"
#include "component_session.h"
#include "pcapmon.h"
#include <echo.h>
#include <memory>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;
using namespace soso;

PcapMon *pmon;

static void usage() {
  cout << "Usage: \n";
  cout << "   -t thread_num\n";
  cout << "   -l log_evel\n";
  cout << "   -b block mode\n";
  cout << "   -p pcap file\n";
  exit(1);
}

void sighandler(int signum) {
  cout << "SIG INT\n";
  pmon->terminate();
  delete pmon;
  exit(signum);
}

int main(int argc, char **argv) {
  int c;
  struct opt {
    int thread_num = 1;
    bool blocking = true;
    int log_level = L_ALL;
    string pcap_source;
  };
  struct opt opt;

  while ((c = getopt(argc, argv, "t:l:p:b")) != -1) {
    switch (c) {
    case 't':
      opt.thread_num = atoi(optarg);
      break;
    case 'l':
      opt.log_level = atoi(optarg);
      break;
    case 'b':
      opt.blocking = false;
      break;
    case 'p':
      opt.pcap_source = optarg;
      break;
    case '?':
      usage();
    }
  }

  if (opt.pcap_source == "") {
    usage();
  }

  signal(SIGINT, sighandler);

  default_echo_level(opt.log_level);

  shared_ptr<ComponentChain> chain = make_shared<ComponentChain>();
  chain->registers(make_shared<Component80211>("80211"));
  chain->registers(make_shared<ComponentSession>("session"));

  pmon = new PcapMon(chain, opt.pcap_source.c_str(), opt.thread_num, 10);
  pmon->run(opt.blocking);

  return 0;
}
