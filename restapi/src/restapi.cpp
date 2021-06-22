#include "restapi.h"
#include <pthread.h>
/*
 * reference
 * https://github.com/Meenapintu/Restweb
 * https://github.com/microsoft/cpprestsdk
 */

using namespace std;
using namespace web;
using namespace http;
using namespace soso;
using namespace utility;
using namespace http::experimental::listener;

RestApi::RestApi(const string &address, const string &name) //
    : _name(name) {
  setAddress(address);
}

RestApi::~RestApi() {
  if (_endpoint != nullptr) {
    on_shutdown();
  }
}

void RestApi::setAddress(const string &address) {
  uri_builder uri(address);
  auto addr = uri.to_uri().to_string();
  pthread_setname_np(pthread_self(), _name.c_str());
  _endpoint = unique_ptr<Endpoint>(new Endpoint(addr));
}

void RestApi::on_initialize() { //
  assert(_endpoint != nullptr);
  _endpoint->open().wait();
}

void RestApi::on_shutdown() { //
  _endpoint->close().wait();
  _endpoint = nullptr;
}