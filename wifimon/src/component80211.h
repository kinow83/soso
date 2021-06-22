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
  Component80211(const std::string &name) : Component(name) {}
  virtual ~Component80211();
  virtual bool init();
  virtual bool prepare(Request *request);
  virtual bool process(Request *request);
  virtual bool post(Request *request);
  virtual void schedule();
};

}; // namespace soso
#endif