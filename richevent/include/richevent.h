#include <czmq.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

class RichEventBase;
/**
 * @brief richevent callback 함수
 *
 */
using rbase_cb_fn = std::function<bool(zsock_t *, RichEventBase *)>;

class RichEventBase {
private:
  zpoller_t *_poller;                       ///< poll
  std::map<zsock_t *, rbase_cb_fn> _cb_map; ///< callback map list

public:
  RichEventBase();

  virtual ~RichEventBase();

  /**
   * @brief 서비스 callback 등록
   *
   * @param endpoint 연결 endpoint 주소
   * @param callback
   * @return true
   * @return false
   */
  bool subscribe(const std::string &endpoint, rbase_cb_fn callback);

  /**
   * @brief blocking run mode
   *
   */
  void event_loop();

  /**
   * @brief non blocking run mode
   *
   * @param ms (millisecond)
   */
  void event_once(int ms);

private:
  void dispatch(int ms);
};
