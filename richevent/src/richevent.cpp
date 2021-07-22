#include "richevent.h"
#include <iostream>

RichEventBase::RichEventBase() {
  // ignore zmq signal
  zsys_handler_set(NULL);
  _poller = zpoller_new(NULL);
}

RichEventBase::~RichEventBase() {
  for (auto it : _cb_map) {
    zsock_t *zsock = it.first;
    zsock_destroy(&zsock);
    zpoller_remove(_poller, zsock);
  }
  zpoller_destroy(&_poller);
}

void RichEventBase::event_loop() {
  while (true) {
    dispatch(-1);
  }
}

void RichEventBase::event_once(int ms) { //
  dispatch(ms);
}

void RichEventBase::dispatch(int ms) {
  zsock_t *reader = (zsock_t *)zpoller_wait(_poller, ms);
  auto cb_iter = _cb_map.find(reader);
  if (cb_iter == _cb_map.end()) {
    zpoller_remove(_poller, reader);
    zsock_destroy(&reader);
  }

  rbase_cb_fn callback = cb_iter->second;
  if (!callback(reader, this)) {
    zpoller_remove(_poller, reader);
    zsock_destroy(&reader);
    _cb_map.erase(cb_iter);
  }
}

bool RichEventBase::subscribe(const std::string &endpoint,
                              rbase_cb_fn callback) {
  SOCKET sockfd;
  zsock_t *reader = nullptr;

  reader = zsock_new_pull(endpoint.c_str());
  if (!reader) {
    return false;
  }
  sockfd = zsock_fd(reader);
  if (sockfd <= 0) {
    zsock_destroy(&reader);
    return false;
  }
  zpoller_add(_poller, reader);

  _cb_map[reader] = callback;
  return true;
}
