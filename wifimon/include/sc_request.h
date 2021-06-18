#ifndef __SOSO_SC_REQUEST_H__
#define __SOSO_SC_REQUEST_H__

#include "request.h"
#include <stdint.h>

namespace soso {
class ScRequest : public Request {
private:
  uint32_t worker_id;
  int sock_fd;

public:
  ScRequest(uint32_t worker_id, int sock_fd) { //
    this->worker_id = worker_id;
    this->sock_fd = sock_fd;
  }

  int getSockFd() { return sock_fd; }
  int getWorkerId() { return worker_id; }
};
}; // namespace soso

#endif