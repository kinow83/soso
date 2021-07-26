#include "richevent_reader.h"
#include <iostream>
#include <string>

using namespace chkchk;
using namespace std;
using namespace nlohmann;

RichEventReader::RichEventReader() {
  // ignore zmq signal
  zsys_handler_set(NULL);
  _poller = zpoller_new(NULL);
  assert(_poller != nullptr);
}

RichEventReader::~RichEventReader() {
  for (auto &it : _readers) {
    zsock_t *zsock = it.first;
    // reader_context_t &ctx = it.second;

    zsock_destroy(&zsock);
    zpoller_remove(_poller, zsock);
  }
  zpoller_destroy(&_poller);
}

void RichEventReader::event_loop() {
  while (true) {
    dispatch(-1);
  }
}

void RichEventReader::event_once(int ms) { //
  dispatch(ms);
}

optional<string> RichEventReader::recv(zsock_t *zsock, reader_context_t &ctx) {
  string data;
  char *msg = nullptr;
  char *service = nullptr;
  size_t size = 0;
  bool ok = false;

  switch (ctx.event_type) {
  case RICH_EVENT_SUB:
    if (zsock_recv(zsock, "sb", &service, &msg, &size) != -1) {
      ok = true;
    }
    break;
  case RICH_EVENT_PULL:
    msg = zstr_recv(zsock);
    if (msg) {
      ok = true;
    }
    break;
  default:
    return nullopt;
  }
  // recv fail
  if (!ok) {
    return nullopt;
  }

  data = string(msg);
  if (msg) {
    free(msg);
  }
  if (service) {
    free(service);
  }
  return make_optional<string>(data);
}

optional<json> RichEventReader::recv_json(zsock_t *zsock,
                                          reader_context_t &ctx) {
  optional<string> data = recv(zsock, ctx);
  if (data.has_value()) {
    json j = json::parse(*data);
    return make_optional<json>(j);
  }
  return nullopt;
}

void RichEventReader::dispatch(int ms) {
  zsock_t *reader = (zsock_t *)zpoller_wait(_poller, ms);
  auto it = _readers.find(reader);
  if (it == _readers.end()) {
    return;
  }

  reader_context_t &ctx = it->second;
  if (!ctx.cb(reader, ctx)) {
    zpoller_remove(_poller, reader);
    zsock_destroy(&reader);
    _readers.erase(it);
  }
}

bool RichEventReader::register_sub(const char *service, const char *endpoint,
                                   rbase_cb_fn callback) {
  return subscribe(RICH_EVENT_SUB, service, endpoint, callback);
}

bool RichEventReader::register_pull(const char *service, const char *endpoint,
                                    rbase_cb_fn callback) {
  return subscribe(RICH_EVENT_PULL, service, endpoint, callback);
}

bool RichEventReader::subscribe(int type, const char *service,
                                const char *endpoint, rbase_cb_fn callback) {
  zsock_t *reader = nullptr;

  switch (type) {
  case RICH_EVENT_SUB:
    reader = zsock_new_sub(endpoint, service);
    break;
  case RICH_EVENT_PULL:
    reader = zsock_new_pull(endpoint);
    break;
  }
  if (!reader) {
    return false;
  }

  zpoller_add(_poller, reader);

  reader_context_t ctx;
  ctx.self = this;
  ctx.event_type = type;
  ctx.cb = callback;
  ctx.endpoint = endpoint;
  ctx.service = service;

  _readers[reader] = ctx;
  return true;
}
