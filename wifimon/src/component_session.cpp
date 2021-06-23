

#include "component_session.h"
#include "echo.h"
#include "nlohmann/json.hpp"
#include "request.h"
#include "wifiutils.h"
#include <assert.h>
#include <fmt/format.h>
#include <optional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;
using namespace fmt;
using namespace web;
using namespace soso;

static long ap_hash_func(ap_info_t &ap) {
  return ap.band +                                 //
         ap.bssid[0] + ap.bssid[1] + ap.bssid[2] + //
         ap.bssid[3] + ap.bssid[4] + ap.bssid[5];
}

static int ap_hash_compare(ap_info_t &old, ap_info_t &_new) {
  return (maccmp(old.bssid, _new.bssid) && (old.band == _new.band)) ? 0 : 1;
}

optional<ap_info_t> ap_copy(ap_info_t &ap) {
  ap_info_t new_ap;
  memcpy(&new_ap, &ap, sizeof(ap_info_t));
  return make_optional<ap_info_t>(new_ap);
}

static long st_hash_func(st_info_t &st) {
  return st.mac[0] + st.mac[1] + st.mac[2] + //
         st.mac[3] + st.mac[4] + st.mac[5];
}

static int st_hash_compare(st_info_t &old, st_info_t &_new) {
  return (maccmp(old.mac, _new.mac)) ? 0 : 1;
}

optional<st_info_t> st_copy(st_info_t &st) {
  st_info_t new_st;
  memcpy(&new_st, &st, sizeof(st_info_t));
  return make_optional<st_info_t>(new_st);
}

#define libfmtmac "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}"
#define libfmtap(ap)                                                           \
  (ap).bssid[0], (ap).bssid[1], (ap).bssid[2], (ap).bssid[3], (ap).bssid[4],   \
      (ap).bssid[5]
#define libfmtst(st)                                                           \
  (st).mac[0], (st).mac[1], (st).mac[2], (st).mac[3], (st).mac[4], (st).mac[5]

string ap_to_string(ap_info_t &ap) {
  //
  return format("{} " libfmtmac " ({})", ap.band, libfmtap(ap), ap.ssid);
}

string st_to_string(st_info_t &st) {
  //
  return format(libfmtmac " ap: " libfmtmac, libfmtap(st), libfmtst(st));
}

bool ComponentSession::init() {
  api = make_shared<RestApi>("http://127.0.0.1:6666", "sess_restapi");

  apDB = make_shared<KHash<ap_info_t>>(65536,           //
                                       ap_hash_func,    //
                                       ap_hash_compare, //
                                       nullptr,         //
                                       ap_copy,         //
                                       ap_to_string,    //
                                       true);
  stDB = make_shared<KHash<st_info_t>>(65536,           //
                                       st_hash_func,    //
                                       st_hash_compare, //
                                       nullptr,         //
                                       st_copy,         //
                                       st_to_string,    //
                                       true);

  // SSID 출력 API
  api->addAPI(API_GET, "/network/ssid",
              [=](const http::http_request &r, const json::value &v) { //
                (void)r;
                (void)v;
                nlohmann::json aps = nlohmann::json::array();
                apDB->loop([&aps](ap_info_t &ap) { //
                  if (ap.ssid_len > 0) {
                    aps += ap.ssid;
                  }
                });
                return aps.dump();
              });

  cout << api->endpointTables() << endl;

  api->on_initialize();

  return true;
}

bool ComponentSession::prepare(Request *request) {
  PcapRequest *R = reinterpret_cast<PcapRequest *>(request);
  (void)R;
  return noop();
}

bool ComponentSession::process(Request *request) {
  PcapRequest *R = reinterpret_cast<PcapRequest *>(request);
  bool inserted;

  if (R->ap.seen && R->ap.ssid_len > 0) {
    inserted = apDB->insert(R->ap);
    if (inserted) {
      ap_to_string(R->ap);
    }
  }
  if (R->sta.seen) {
    inserted = stDB->insert(R->sta);
    if (inserted) {
      st_to_string(R->sta);
    }
  }
  if (R->remote.seen) {
    inserted = stDB->insert(R->remote);
    if (inserted) {
      st_to_string(R->remote);
    }
  }
  return true;
}

bool ComponentSession::post(Request *request) {
  PcapRequest *R = reinterpret_cast<PcapRequest *>(request);
  (void)R;

  return noop();
}

void ComponentSession::schedule() { //
  apDB->show(NULL);
}

void ComponentSession::showAP() {
  //
}
void ComponentSession::showST() {
  //
}

ComponentSession::~ComponentSession() {
  //
  api->on_shutdown();
}