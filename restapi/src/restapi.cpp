#include "restapi.h"

/*
 * reference
 * https://github.com/Meenapintu/Restweb
 * https://github.com/microsoft/cpprestsdk
 */

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

RestApi::RestApi() { //
  _endpoint = nullptr;
}

RestApi::RestApi(const std::string &address) : _address(address) { //
  setAddress(address);
}

RestApi::~RestApi() {
  if (_endpoint != nullptr) {
    on_shutdown();
  }
}

void RestApi::setAddress(const std::string &address) {
  uri_builder uri(address);
  auto addr = uri.to_uri().to_string();
  _endpoint = std::unique_ptr<Endpoint>(new Endpoint(addr));
}

void RestApi::on_initialize() { //
  assert(_endpoint != nullptr);
  _endpoint->open().wait();
}

void RestApi::on_shutdown() { //
  _endpoint->close().wait();
  _endpoint = nullptr;
}