#ifndef __IEEE_80211_H__
#define __IEEE_80211_H__

#include "ieee80211_elements.h"
#include <stdint.h>

enum ht_capability_info_t {
  ELEMENT_ID_POS = 0,
  LENGTH_POS = 1,
  PREV_BYTE_POS = 2,
  NEXT_BYTE_POS = 3,

  INFO_BIT_0 = 0, /* Bit 0 : LDPC Coding Capability - Indicating Low Density
                     Parity Check usage. */
  INFO_BIT_1, /* Bit 1 : Supported Channel Width– 0 for only 20MHz, 1 for both
                 20MHz & 40MHz */
  INFO_BIT_2, /* Bit 2-3 : SM Power Save – to indicate STA SM Power Save
                 capability. Here are the possbile values & their
                 interpretation.*/
  INFO_BIT_3,
  INFO_BIT_4, /* Bit 4 : HT Greenfield – When set to 1 STA is capable of
                 receiving HT Greenfield PPDU. */
  INFO_BIT_5, /* Bit 5 : Short GI for 20MHz – STA capability to receive frames
                 with a short GI in 20MHz */
  INFO_BIT_6, /* Bit 6 : Short GI for 40MHz – STA capability to receive frames
                 with a short GI in 40MHz */
  INFO_BIT_7, /* Bit 7 : TX STBC – STA capability of transmitting PPDU using
                 STBC (Space Time Block Coding) */

  INFO_BIT_8 = 0,
  INFO_BIT_9,
  INFO_BIT_10,
  INFO_BIT_11,
  INFO_BIT_12,
  INFO_BIT_13,
  INFO_BIT_14,
  INFO_BIT_15
};

enum h80211_type {
  packet_type_management = 0,
  packet_type_control = 1,
  packet_type_data = 2
};

enum h80211_subtype {
  // Management subtypes
  packet_subtype_association_req = 0,
  packet_subtype_association_resp = 1,
  packet_subtype_reassociation_req = 2,
  packet_subtype_reassociation_resp = 3,
  packet_subtype_probe_req = 4,
  packet_subtype_probe_resp = 5,
  packet_subtype_timing_advertisement = 6,
  packet_subtype_beacon = 8,
  packet_subtype_atim = 9,
  packet_subtype_disassociation = 10,
  packet_subtype_authentication = 11,
  packet_subtype_deauthentication = 12,
  packet_subtype_action = 13,
  // Phy subtypes
  packet_subtype_trigger = 2,
  packet_subtype_ndp_announcement = 5,
  packet_subtype_block_ack_req = 8,
  packet_subtype_block_ack = 9,
  packet_subtype_pspoll = 10,
  packet_subtype_rts = 11,
  packet_subtype_cts = 12,
  packet_subtype_ack = 13,
  packet_subtype_cf_end = 14,
  packet_subtype_cf_end_ack = 15,
  // Data subtypes
  packet_subtype_data = 0,
  packet_subtype_data_cf_ack = 1,
  packet_subtype_data_cf_poll = 2,
  packet_subtype_data_cf_ack_poll = 3,
  packet_subtype_data_null = 4,
  packet_subtype_cf_ack = 5,
  packet_subtype_cf_ack_poll = 6,
  packet_subtype_data_qos_data = 8,
  packet_subtype_data_qos_data_cf_ack = 9,
  packet_subtype_data_qos_data_cf_poll = 10,
  packet_subtype_data_qos_data_cf_ack_poll = 11,
  packet_subtype_data_qos_null = 12,
  packet_subtype_data_qos_cf_poll_nod = 14,
  packet_subtype_data_qos_cf_ack_poll = 15
};

enum h80211_disttype { //
  distrib_unknown,
  distrib_from,
  distrib_to,
  distrib_inter,
  distrib_adhoc
};

enum channel_band {
  channel_unkown,
  channel_band2,
  channel_band5,
};

enum phy_carrier_type {
  carrier_unknown,
  carrier_80211b,
  carrier_80211bplus,
  carrier_80211a,
  carrier_80211g,
  carrier_80211fhss,
  carrier_80211dsss,
  carrier_80211n20,
  carrier_80211n40,
  carrier_80211ac80,
  carrier_80211ac160
};

typedef struct {
#if __BYTE_ORDER == __BIG_ENDIAN
  unsigned short subtype : 4;
  unsigned short type : 2;
  unsigned short version : 2;

  unsigned short order : 1;
  unsigned short wep : 1;
  unsigned short more_data : 1;
  unsigned short power_management : 1;

  unsigned short retry : 1;
  unsigned short more_fragments : 1;
  unsigned short from_ds : 1;
  unsigned short to_ds : 1;
#else
  unsigned short version : 2;
  unsigned short type : 2;
  unsigned short subtype : 4;

  unsigned short to_ds : 1;
  unsigned short from_ds : 1;
  unsigned short more_fragments : 1;
  unsigned short retry : 1;

  unsigned short power_management : 1;
  unsigned short more_data : 1;
  unsigned short wep : 1;
  unsigned short order : 1;
#endif
} __attribute__((packed)) frame_control_t;

typedef struct {
#if __BYTE_ORDER == __BIG_ENDIAN
  unsigned char channel_agility : 1;
  unsigned char pbcc : 1;
  unsigned char short_preamble : 1;
  unsigned char privacy : 1;
  unsigned char cfpoll_request : 1;
  unsigned char cfpoll_able : 1;
  unsigned char ibss : 1;
  unsigned char ess : 1;

  unsigned char rsvd5 : 1;
  unsigned char rsvd4 : 1;
  unsigned char dsss_ofdm : 1;
  unsigned char rsvd3 : 1;
  unsigned char rsn_enabled : 1;
  unsigned char g_short_slottime : 1;
  unsigned char rsvd2 : 1;
  unsigned char rsvd1 : 1;
#else
  unsigned char ess : 1;
  unsigned char ibss : 1;
  unsigned char cfpoll_able : 1;
  unsigned char cfpoll_request : 1;
  unsigned char privacy : 1;
  unsigned char short_preamble : 1;
  unsigned char pbcc : 1;
  unsigned char channel_agility : 1;

  unsigned char rsvd1 : 1;
  unsigned char rsvd2 : 1;
  unsigned char g_short_slottime : 1;
  unsigned char rsn_enabled : 1;
  unsigned char rsvd3 : 1;
  unsigned char dsss_ofdm : 1;
  unsigned char rsvd4 : 1;
  unsigned char rsvd5 : 1;
#endif
} __attribute__((packed)) capability_info_t;

typedef struct {
#if __BYTE_ORDER == __BIG_ENDIAN
  unsigned short seqnum : 12;
  unsigned short flag : 4;
#else
  unsigned short flag : 4;
  unsigned short seqnum : 12;
#endif
} __attribute__((packed)) fragseq_t;

typedef struct {
  uint8_t t;
  uint8_t l;
  uint8_t v[0];
} tag_t;

#endif