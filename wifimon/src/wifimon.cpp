
#include "component80211.h"
#include "pcapmon.h"
#include <echo.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

using namespace std;
using namespace soso;

static void usage() {
  cout << "Usage: \n";
  cout << "   -t thread_num\n";
  cout << "   -l log_evel\n";
  cout << "   -b block mode\n";
  cout << "   -p pcap file\n";
  exit(1);
}

int main(int argc, char **argv) {
  int c;
  struct opt {
    int thread_num = 1;
    bool blocking = true;
    int log_level = L_ALL;
    string pcapfile;
  };
  struct opt opt;

  while ((c = getopt(argc, argv, "t:l:bp:")) != -1) {
    switch (c) {
    case 't':
      opt.thread_num = atoi(optarg);
      break;
    case 'l':
      opt.log_level = atoi(optarg);
      break;
    case 'f':
      opt.blocking = false;
      break;
    case 'p':
      opt.pcapfile = optarg;
      break;
    case '?':
      usage();
    }
  }

  default_echo_level(opt.log_level);

  ComponentChain chain;
  chain.registers(new Component80211("80211"));
  // chain.registers(make_shared<ComponentSession>("session"));

  PcapMon mon(&chain, opt.pcapfile.c_str(), opt.thread_num);

  mon.run(opt.blocking);

  return 0;
}
