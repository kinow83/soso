#ifndef __SOSO_ENDPOINT__
#define __SOSO_ENDPOINT__

#include "cpprest/asyncrt_utils.h"
#include "cpprest/containerstream.h"
#include "cpprest/filestream.h"
#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include "cpprest/uri.h"
#include <functional>
#include <map>

namespace soso {

/**
 * @brief rest api 수행 함수 타입
 */
typedef std::function<       //
    const utility::string_t( //
        const web::http::http_request &, const web::json::value &)>
    endpoint_handler_t;

/**
 * @brief API 호출 시 HTTP Method 정의
 */
enum API_METHOD { //
  API_GET = 0,
  API_PUT,
  API_POST,
  API_DEL,
  API_METHOD_SIZE
};

/**
 * @brief Endpoint
 */
class Endpoint {

private:
  void get(web::http::http_request message);
  void put(web::http::http_request message);
  void post(web::http::http_request message);
  void del(web::http::http_request message);
  void callapi(API_METHOD method, web::http::http_request message);

  /// cpprestsdk API 서버 리스너
  web::http::experimental::listener::http_listener _listener;
  /// API 동작 정의 테이블 (GET/POST/PUT/DEL 각 각 정의)
  std::map<utility::string_t, endpoint_handler_t> _tables[API_METHOD_SIZE];
  ///
  size_t _listen_thread_num;

public:
  Endpoint(utility::string_t url, size_t listen_thread_num = 4);
  ~Endpoint() {}

  std::string endpointTables() {
    int i = 0;
    std::string result = "[Endpoint::report]\n";
    for (auto &table : _tables) {
      for (auto &m : table) {
        auto get_path_name = [](int i) {
          switch (i) {
          case 0:
            return "GET";
          case 1:
            return "PUT";
          case 2:
            return "POST";
          case 3:
            return "DEL";
          default:
            return "";
          }
        };

        result += get_path_name(i);
        result += ":" + m.first + "\n";
      }
      i++;
    }
    return result;
  }

  /**
   * @brief rest api 서버 시작
   * @return pplx::task<void>
   */
  pplx::task<void> open() { return _listener.open(); }

  /**
   * @brief rest api 서버 종료
   * @return pplx::task<void>
   */
  pplx::task<void> close() { return _listener.close(); }

  /**
   * @brief rest api 서비스 등록
   * @param method GET, PUT, POST, DEL
   * @param path endpoint URL
   * @param handle rest api 동작 함수
   * @return true
   * @return false
   */
  bool addEndpoint(API_METHOD method,            //
                   const utility::string_t path, //
                   endpoint_handler_t handle);

  /**
   * @brief rest api endpoint 핸들 반환
   * @param method
   * @param path
   * @return endpoint_handler_t*
   */
  endpoint_handler_t *           //
  getEndpoint(API_METHOD method, //
              const utility::string_t path);
};
}; // namespace soso
#endif