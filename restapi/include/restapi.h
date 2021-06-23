#ifndef __SOSO_REST_API__
#define __SOSO_REST_API__

#include "endpoint.h"
#include <iostream>

namespace soso {

/**
 * @brief RestApi 정의된 API를 추가하고 서버를 동작시킨다.
 */
class RestApi {
private:
  std::string _name;
  /// API 서버 주소 (포트까지 포함)
  std::string _address;
  /// URL에 맵핑된 API 호출 동작 정의 객체
  std::unique_ptr<Endpoint> _endpoint;

public:
  /**
   * @brief RestAPI 생성자
   * @param address
   */
  RestApi(const std::string &address, const std::string &name);

  /**
   * @brief RestAPI 소멸자
   */
  ~RestApi();

  std::string endpointTables() { return _endpoint->endpointTables(); }

  /**
   * @brief API url 주소 입력
   * @param address
   */
  void setAddress(const std::string &address);

  /**
   * @brief API 서버 실행
   */
  void on_initialize();

  /**
   * @brief API 서버 종료
   */
  void on_shutdown();

  /**
   * @brief API 정의 추가 (HTTP Method별 URL에 맵핑되는 API 동작 정의 추가)
   * @param method GET/POST/PUT/DEL
   * @param path url (endpoint)
   * @param handle API 호출 시 동작하는 함수 정의
   * @return true
   * @return false_api_handler
   */
  bool addAPI(API_METHOD method,            //
              const utility::string_t path, //
              endpoint_handler_t handle) {
    return _endpoint->addEndpoint(method, path, handle);
  }
};
}; // namespace soso

#endif