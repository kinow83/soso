#include "nlohmann/json.hpp"
#include "restapi.h"
#include <iostream>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace soso;

int main() {
  RestApi api("http://127.0.0.1:8888", "test");
  auto f = [](const http::http_request &r, const json::value &v) {
    (void)r;
    nlohmann::json j;

    if (!v.is_null()) {
      return v.to_string();
    }
    j["result"] = "no message";
    return j.dump();
  };

  api.addAPI(API_GET, "/echo/name", f);
  api.addAPI(API_POST, "/echo/name", f);
  api.on_initialize();
  getchar();
  api.on_shutdown();
}