#ifndef __SOSO_REQUEST_H__
#define __SOSO_REQUEST_H__

#include <pcap.h>
#include <stdint.h>
#include <stdio.h>

namespace soso {
/**
 * @brief 모듈 컴포넌트에 전달되는 정보 Class\n
 * 수신된 패킷 또는 메타정보등을 추가할 수 있다.\n
 * Request Class를 상속 받아 구현에 맞게 커스터마이징 필요
 */
class Request {
private:
  size_t _request_id;

public:
  Request(size_t id = 0) : _request_id(id) {}
  void setRequestID(size_t id) { _request_id = id; }
  size_t getRequestID() { return _request_id; }
};

}; // namespace soso
#endif