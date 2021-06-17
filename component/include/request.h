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
  size_t getRequestID() { return _request_id; }
};

/**
 * @brief server-client 모델 request 정의
 */
class ScRequest : public Request {
private:
  int _sock_fd;

public:
  ScRequest(int sock_fd) { //
    this->_sock_fd = sock_fd;
  }

  int getSock() { return _sock_fd; }
};

#define MAX_PCAP_CAPSIZE 1500

/**
 * @brief pcap 모델 request 정의
 */
class PcapRequest : public Request {
public:
  size_t _pkt_id;
  struct pcap_pkthdr _pcap_pkthdr;
  u_char _packet[MAX_PCAP_CAPSIZE];
};

}; // namespace soso
#endif