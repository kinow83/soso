#include "endpoint.h"
#include <cpprest/http_listener.h>
#include <pplx/threadpool.h>

using namespace std;
using namespace web;
using namespace http;
using namespace soso;
using namespace utility;
using namespace http::experimental::listener;

Endpoint::Endpoint(utility::string_t url, size_t listen_thread_num) //
    : _listener(url),                                               //
      _listen_thread_num(listen_thread_num) {
  /// TODO:
  /// https://github.com/microsoft/cpprestsdk/blob/master/Release/src/pplx/threadpool.cpp
  /// initialize_shared_threadpool
  /// std::call_once
  crossplat::threadpool::initialize_with_threads(_listen_thread_num);

  _listener.support(methods::GET, //
                    bind(&Endpoint::get, this, placeholders::_1));
  _listener.support(methods::PUT, //
                    bind(&Endpoint::put, this, placeholders::_1));
  _listener.support(methods::POST, //
                    bind(&Endpoint::post, this, placeholders::_1));
  _listener.support(methods::DEL, //
                    bind(&Endpoint::del, this, placeholders::_1));
}

static json::value &appendJSON( //
    json::value &json, const map<string_t, string_t> &map) {

  for (auto m : map) {
    json[m.first] = json::value::parse(m.second);
  }
  return json;
}

bool Endpoint::addEndpoint(API_METHOD method,            //
                           const utility::string_t path, //
                           endpoint_handler_t handle) {
  auto &endpoint = _tables[method];
  if (endpoint.find(path) != endpoint.end()) {
    return false;
  }
  endpoint[path] = handle;
  return true;
}

endpoint_handler_t *Endpoint::getEndpoint(API_METHOD method, //
                                          const utility::string_t path) {
  auto &endpoint = _tables[method];
  if (endpoint.find(path) == endpoint.end()) {
    return nullptr;
  }
  return &endpoint[path];
}

void Endpoint::post(http_request message) {
  return callapi(API_POST, message);
};

void Endpoint::del(http_request message) { return callapi(API_DEL, message); };

void Endpoint::put(http_request message) { return callapi(API_PUT, message); };

void Endpoint::get(http_request message) { return callapi(API_GET, message); }

void Endpoint::callapi(API_METHOD method, http_request message) {
  endpoint_handler_t *endpoint = NULL;
  string_t url = http::uri::decode(message.relative_uri().path());
  string_t resp = "";

  endpoint = getEndpoint(method, url);
  if (endpoint) {
    /// parameters
    auto p = message.extract_json().get();
    /// query
    const auto &q = http::uri::split_query(message.relative_uri().query());
    /// merge parameters + query
    resp = (*endpoint)(message, appendJSON(p, q));
  }

  if (resp == "") {
    // ucout << "404" << endl;
    message.reply(status_codes::NotFound);
  } else {
    // ucout << resp << endl;
    message.reply(status_codes::OK, resp);
  }
}
