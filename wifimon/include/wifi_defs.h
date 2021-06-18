#ifndef __SOSO_WIFI_DEFS_H__
#define __SOSO_WIFI_DEFS_H__

enum wifi_80211_type {
  wifi_type_management = 0,
  wifi_type_control = 1,
  wifi_type_data = 2
};

enum wifi_80211_subtype {
  // Management subtypes
  wifi_subtype_association_req = 1,
  wifi_subtype_association_resp = 2,
  wifi_subtype_reassociation_req = 3,
  wifi_subtype_reassociation_resp = 4,
  wifi_subtype_probe_req = 5,
  wifi_subtype_probe_resp = 6,
  wifi_subtype_beacon = 7,
  wifi_subtype_disassociation = 8,
  wifi_subtype_authentication = 9,
  wifi_subtype_deauthentication = 10,
  wifi_subtype_action = 11,
  // wifi_subtype_timing_advertisement = 12,
  // wifi_subtype_atim = 13,

  // Phy subtypes
  wifi_subtype_cts = 1,
  wifi_subtype_rts = 2,
  wifi_subtype_block_ack_req = 3,
  wifi_subtype_block_ack = 4,
  wifi_subtype_pspoll = 5,
  wifi_subtype_trigger = 6,
  wifi_subtype_ndp_announcement = 7,
  // wifi_subtype_ack = 8,

  // Data subtypes
  wifi_subtype_data = 1,
  wifi_subtype_data_null = 2,
};

#endif