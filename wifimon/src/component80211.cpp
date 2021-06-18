#include "component80211.h"
#include "echo.h"
#include "wifi_defs.h"
#include "wifiutils.h"
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace soso;
using namespace std;

static bool vaild_pkt_len(int min, int max, int pkt_len) {
  if (pkt_len < min || pkt_len > max) {
    return false;
  }
  return true;
}

const uint8_t *Component80211::parserRadio(PcapRequest *request,
                                           const struct pcap_pkthdr *header,
                                           const radiotap_t *radiotap) {
  // radiotap info

  // phy_info.channel = freq_to_channel(radiotap.);

  // physical info

  // packet info
  request->pkt_len.len = header->len;       // real packet length
  request->pkt_len.caplen = header->caplen; // captured packet length
  request->pkt_len.framelen =
      header->len - radiotap->it_len; // real frame length
  request->pkt_len.framecaplen =
      header->caplen - radiotap->it_len; // captured frame length
  return ((uint8_t *)radiotap) + radiotap->it_len;
}
const uint8_t *
Component80211::parserFrameControl(PcapRequest *request,
                                   const frame_control_t *fc_packet) {
  request->fc = *fc_packet;
  return ((uint8_t *)fc_packet) + sizeof(frame_control_t);
}

void Component80211::parserTaggedParameters(PcapRequest *request,
                                            const uint8_t *tag_packet,
                                            size_t packetlen) {
  tag_param_t *tag;
  int offset = 0;
  size_t length = packetlen;

  while (length > 2) {
    tag = (tag_param_t *)(tag_packet + offset);
    if ((size_t)tag->length + 2 > length) {
      break;
    }

    switch (tag->type) {
    case wlan_tag_ssid:
      // echo.t("[SSID] offset:%d, len:%d, tag: %d\n", offset, length,
      // tag->length);
      request->ap.ssid_len =
          ssidcpy(request->ap.ssid, (char *)tag->value, tag->length);
      request->ap.ssid_broadcast = 1;
      break;
    }

    offset += (tag->length + 2);
    length -= (tag->length + 2);
    // echo.t("offset:%d, len:%d\n", offset, length);
  }
}

static inline const uint8_t *posmov(const uint8_t **pos, int mov) {
  const uint8_t *cur = *pos;
  *pos += mov;
  return cur;
}

bool Component80211::parserManagementFrame(PcapRequest *request,
                                           const uint8_t *frame,
                                           size_t frame_len) {
  const uint8_t *pos = frame;
  capability_info_t *capa = NULL;
  const uint8_t *da, *sa, *bssid;

  capa = capa;

  if (!vaild_pkt_len(24, 1500, request->pkt_len.framecaplen)) {
    return false;
  }

  // addr inforequest
  da = posmov(&pos, 6);
  sa = posmov(&pos, 6);
  bssid = posmov(&pos, 6);
  posmov(&pos, 2); // skip fragment number, sequence number

  if (maccmp(sa, bssid)) {
    // AP -> STA
    if (!is_nullmac(bssid) && !is_broadcastmac(bssid)) {
      request->ap.seen = true;
      maccpy(request->ap.bssid, bssid);
    }
    if (!is_nullmac(da) && !is_broadcastmac(da)) {
      request->sta.seen = true;
      maccpy(request->sta.mac, da);
    }
  } else {
    // STA -> AP
    if (!is_nullmac(bssid) && !is_broadcastmac(bssid)) {
      request->ap.seen = true;
      maccpy(request->ap.bssid, bssid);
    }
    if (!is_nullmac(sa) && !is_broadcastmac(sa)) {
      request->sta.seen = true;
      maccpy(request->sta.mac, da);
    }
  }

  // subtype
  switch (request->fc.subtype) {
  case packet_subtype_beacon:
    posmov(&pos, 8); // skip timestamp
    posmov(&pos, 2); // beacon interval
    capa = (capability_info_t *)posmov(&pos, 2);
    parserTaggedParameters(request, pos, frame_len - (pos - frame));
    break;
  case packet_subtype_probe_resp:
    break;
  case packet_subtype_association_resp:
    break;
  case packet_subtype_reassociation_resp:
    break;
  case packet_subtype_authentication:
    break;
  case packet_subtype_action:
    break;
  case packet_subtype_association_req:
    break;
  }

  return true;
}
bool Component80211::parserControlFrame(PcapRequest *request, //
                                        const uint8_t *frame,
                                        size_t frame_len) {
  request = request;
  frame = frame;
  frame_len = frame_len;
  return true;
}
bool Component80211::parserDataFrame(PcapRequest *request, //
                                     const uint8_t *frame, size_t frame_len) {
  request = request;
  frame = frame;
  frame_len = frame_len;
  return true;
}

bool Component80211::init() { //
  return noop();
}

bool Component80211::prepare(Request *request) {
  request = request;
  return noop();
}

bool Component80211::process(Request *request) {
  PcapRequest *pcapRequest = reinterpret_cast<PcapRequest *>(request);
  const uint8_t *frame;
  size_t frame_len;
  bool parse_ok = true;

  frame = parserRadio(pcapRequest, &pcapRequest->pcap_pkthdr,
                      (radiotap_t *)pcapRequest->packet);
  frame = parserFrameControl(pcapRequest, (const frame_control_t *)frame);
  frame += 2; // skip Duration
  frame_len = pcapRequest->pkt_len.framecaplen - (frame - pcapRequest->packet);

  switch (pcapRequest->fc.type) {
  case packet_type_management:
    // echo.d("[MGNT] #%d, frame: %d, caplen: %d", pcapRequest->pkt_id,
    // pcapRequest->pkt_len.framelen,pcapRequest->pkt_len.framecaplen);
    break;
  case packet_type_control:
    break;
  case packet_type_data:
    break;
  }

  switch (pcapRequest->fc.type) {
  case packet_type_management:
    parse_ok = parserManagementFrame(pcapRequest, frame, frame_len);
    break;
  case packet_type_control:
    parse_ok = parserControlFrame(pcapRequest, frame, frame_len);
    break;
  case packet_type_data:
    parse_ok = parserDataFrame(pcapRequest, frame, frame_len);
    break;
  default:
    parse_ok = false;
  }

  return parse_ok;
}

bool Component80211::post(Request *request) { //
  PcapRequest *pcapRequest = reinterpret_cast<PcapRequest *>(request);

  if (pcapRequest->ap.seen) {
    echo.t("(%d) %s (%d)", pcapRequest->ap.ssid_broadcast ? 1 : 0,
           pcapRequest->ap.ssid, pcapRequest->ap.ssid_len);
  }
  return noop();
}

void Component80211::schedule() {
  //
}

Component80211::~Component80211() {
  //
}