#include "nlohmann/json.hpp"
#include <czmq.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace chkchk {

class RichEventWriter;

typedef struct _writer_context_t {
  RichEventWriter *self;
  int event_type;
  std::string endpoint;
  std::string service;
  zsock_t *zsock;
} writer_context_t;

class RichEventWriter {
private:
  enum {
    RICH_EVENT_PUB,
    RICH_EVENT_PUSH,
  };
  std::map<std::string, writer_context_t> _writers;
  bool _auto_connection;

public:
  RichEventWriter(bool auto_connection = false);
  virtual ~RichEventWriter();
  bool register_pub(const char *service, const char *endpoint);
  bool register_push(const char *service, const char *endpoint);
  bool send(const char *service, const char *msg);
  bool send_json(const char *service, nlohmann::json &j);

private:
  bool register_writer(int type, const char *service, const char *endpoint);
};

}; // namespace chkchk