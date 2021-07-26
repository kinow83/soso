#include "nlohmann/json.hpp"
#include <czmq.h>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace chkchk {

class RichEventReader;
struct _reader_context_t;

using rbase_cb_fn = std::function<bool(zsock_t *, struct _reader_context_t &)>;

typedef struct _reader_context_t {
  RichEventReader *self;
  int event_type;
  rbase_cb_fn cb;
  std::string endpoint;
  std::string service;
} reader_context_t;

class RichEventReader {
private:
  enum {
    RICH_EVENT_SUB,
    RICH_EVENT_PULL,
  };

private:
  zpoller_t *_poller;
  std::map<zsock_t *, reader_context_t> _readers;

public:
  RichEventReader();
  virtual ~RichEventReader();
  bool register_sub(const char *service, const char *endpoint,
                    rbase_cb_fn callback);
  bool register_pull(const char *service, const char *endpoint,
                     rbase_cb_fn callback);
  static std::optional<std::string> recv(zsock_t *zsock, reader_context_t &ctx);
  static std::optional<nlohmann::json> recv_json(zsock_t *zsock,
                                                 reader_context_t &ctx);
  void event_loop();
  void event_once(int ms);

private:
  bool subscribe(int type, const char *service, const char *endpoint,
                 rbase_cb_fn callback);
  void dispatch(int ms);
};

}; // namespace chkchk