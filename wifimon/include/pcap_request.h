#ifndef __PCAP_REQUEST_H__
#define __PCAP_REQUEST_H__

#include "ieee80211.h"
#include "request.h"
#include <pcap.h>
#include <stdint.h>

namespace soso {

#define MAX_PCAP_CAPSIZE 1500

typedef struct ap_info_t {
  bool seen;
  uint8_t band;
  uint8_t bssid[6];
  int channel;
  char ssid[33];
  size_t ssid_len;
  uint8_t ssid_broadcast;
} ap_info_t;

typedef struct st_info_t {
  bool seen;
  uint8_t mac[6];
  uint8_t band;
  uint8_t bssid[6];
} st_info_t;

typedef struct pkt_len_t {
  uint32_t len;
  uint32_t caplen;
  uint32_t framelen;
  uint32_t framecaplen;
} pkt_len_t;

class PcapRequest : public Request {
public:
  struct pcap_pkthdr pcap_pkthdr;
  u_char packet[MAX_PCAP_CAPSIZE];

  pkt_len_t pkt_len;
  frame_control_t fc;

  ap_info_t ap;
  st_info_t sta;
  st_info_t remote;

public:
  PcapRequest() : Request() {}
  PcapRequest(const struct pcap_pkthdr *pheader, const u_char *ppacket)
      : Request(0) {
    memcpy(&pcap_pkthdr, pheader, sizeof(pcap_pkthdr));
    memcpy(packet, ppacket, sizeof(packet));
  }
};
}; // namespace soso
#endif