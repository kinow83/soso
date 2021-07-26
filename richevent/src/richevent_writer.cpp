#include "richevent_writer.h"
#include <iostream>
#include <string>

using namespace chkchk;
using namespace nlohmann;
using namespace std;

RichEventWriter::RichEventWriter(bool auto_connection) {
  _auto_connection = auto_connection;
  // ignore zmq signal
  zsys_handler_set(NULL);
}

RichEventWriter::~RichEventWriter() {
  for (auto &it : _writers) {
    writer_context_t &ctx = it.second;
    zsock_destroy(&ctx.zsock);
  }
}

bool RichEventWriter::send_json(const char *service, json &j) {
  return send(service, j.dump().c_str());
}

bool RichEventWriter::send(const char *service, const char *msg) {
  bool ok = false;
  auto it = _writers.find(service);
  if (it == _writers.end()) {
    return false;
  }

  writer_context_t &ctx = it->second;
  switch (ctx.event_type) {
  case RICH_EVENT_PUB:
    if (zsock_send(ctx.zsock, "ss", service, msg) != -1) {
      ok = true;
    }
    break;
  case RICH_EVENT_PUSH:
    if (zstr_send(ctx.zsock, msg) != -1) {
      ok = true;
    }
    break;
  }

  if (!ok) {
    zsock_destroy(&ctx.zsock);
    _writers.erase(it);
    return false;
  }

  return true;
}

bool RichEventWriter::register_pub(const char *service, const char *endpoint) {
  return register_writer(RICH_EVENT_PUB, service, endpoint);
}

bool RichEventWriter::register_push(const char *service, const char *endpoint) {
  return register_writer(RICH_EVENT_PUSH, service, endpoint);
}

bool RichEventWriter::register_writer(int type, const char *service,
                                      const char *endpoint) {
  zsock_t *writer = nullptr;

  switch (type) {
  case RICH_EVENT_PUB:
    writer = zsock_new_pub(endpoint);
    break;
  case RICH_EVENT_PUSH:
    writer = zsock_new_push(endpoint);
    break;
  }
  if (!writer) {
    return false;
  }

  writer_context_t ctx;
  ctx.self = this;
  ctx.endpoint = endpoint;
  ctx.service = service;
  ctx.zsock = writer;
  ctx.event_type = type;

  _writers[service] = ctx;
  return true;
}
