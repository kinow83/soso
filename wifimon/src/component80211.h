#ifndef __SOSO_COMPONENT_80211_H__
#define __SOSO_COMPONENT_80211_H__

#include "component.h"
#include "hstruct.h"
#include "ieee80211.h"
#include "pcap_request.h"
#include "radiotap.h"

namespace soso {

class Component80211 : public Component {
private:
  const uint8_t *parserRadio(PcapRequest *request,             //
                             const struct pcap_pkthdr *header, //
                             const radiotap_t *radiotap);
  const uint8_t *parserFrameControl(PcapRequest *request, //
                                    const frame_control_t *fc_packet);
  bool parserManagementFrame(PcapRequest *request, //
                             const uint8_t *frame, size_t frame_len);
  bool parserControlFrame(PcapRequest *request, //
                          const uint8_t *frame, size_t frame_len);
  bool parserDataFrame(PcapRequest *request, //
                       const uint8_t *frame, size_t frame_len);
  void parserTaggedParameters(PcapRequest *request, //
                              const uint8_t *tag_packet, size_t packetlen);
  bool verifyPacketLength(PcapRequest *request);

public:
  Component80211(std::string name) : Component(name) {}
  ~Component80211();
  bool init();
  bool prepare(Request *request);
  bool process(Request *request);
  bool post(Request *request);
  void schedule();
};

}; // namespace soso
#endif