#ifndef __SOSO_COMPONENT_H__
#define __SOSO_COMPONENT_H__

#include "request.h"
#include <chronos.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace soso {

enum COMPONENT_IDX {
  COMPONENT_INIT = 0,
  COMPONENT_PREPARE,
  COMPONENT_PROCESS,
  COMPONENT_POST,
  COMPONENT_SCHEDULE,
  COMPONENT_SIZE
};

/**
 * @brief 컴포넌트 구성 요소 Class
 */
class Component {
private:
  /// 컴포넌트 이름
  const std::string _name;

protected:
  /// No operation (Next 컴포넌트 호출)
  bool noop() { return true; }

public:
  /**
   * @brief Component 생성자
   * @param name
   */
  Component(std::string name) : _name(name) { //
  }
  /**
   * @brief 컴포넌트 이름 반환
   * @return const std::string
   */
  const std::string getName() { return this->_name; }

  /**
   * @brief Component 소멸자 가상 멤버 함수
   */
  virtual ~Component() {}

  /**
   * @brief 컴포넌트 초기화 가상 멤버 함수
   * @return true
   * @return false
   */
  virtual bool init() = 0;

  /**
   * @brief 컴포넌트 사전 처리 가상 멤버 함수
   * @param request
   * @return true
   * @return false
   */
  virtual bool prepare(std::shared_ptr<Request> request) = 0;

  /**
   * @brief 컴포넌트 수행 가상 멤버 함수
   * @param request
   * @return true
   * @return false
   */
  virtual bool process(std::shared_ptr<Request> request) = 0;

  /**
   * @brief 컴포넌트 후처리 가상 멤버 함수
   * @param request
   * @return true
   * @return false
   */
  virtual bool post(std::shared_ptr<Request> request) = 0;

  /**
   * @brief 컴포넌트 스케줄러 가상 멤버 함수\n
   * 스케줄링 작업이 필요한 경우 구현\n
   * 컴포넌트 호출 시 이전 컴포넌트의 성공 여부와 상관없이 무조건 호출
   */
  virtual void schedule() = 0;
};

/**
 * @brief 컴포넌트을 체인으로 관리하여 각 컴포넌트을 호출 및 관리하는 Class
 */
class ComponentChain {
private:
  std::vector<ChronosStack> _chrono_stack;
  bool _trace_chrono = true;

  /// 컴포넌트 구성요청 체인 리스트
  std::vector<std::shared_ptr<Component>> _chains;

public:
  ComponentChain();

  ~ComponentChain() = default;

  size_t componentSize() { return _chains.size(); }

  const std::string getChronoResult() {
    std::string results = "";

    for (ChronosStack &c : _chrono_stack) {
      results += c.toString();
    }
    return results;
  }

  void chronosMonitoring(COMPONENT_IDX comp_idx,
                         std::function<void(ChronosStack &)> f);

  /**
   * @brief 컴포넌트 추가
   * @param component
   */
  void registers(std::shared_ptr<Component> component);

  /**
   * @brief 컴포넌트 접근자
   * @param name
   * @return std::shared_ptr<Component>
   */
  std::shared_ptr<Component> operator[](const std::string name);

  /**
   * @brief 컴포넌트 제거
   * @param name 등록 시 사용한 컴포넌트 이름
   */
  void remove(const std::string name);

  /**
   * @brief 컴포넌트 초기화
   * 주의: 자원 초기화 시 multi-thread로 동작하지 않도록 해야 함.
   * @return true
   * @return false
   */
  bool initComponent();

  /**
   * @brief 컴포넌트 호출\n
   * 호출 순서: prepare -> process -> post
   * @param request
   */
  void callComponent(std::shared_ptr<Request> request);

  /**
   * @brief 컴포넌트의 schedule(단일 스레드로 동작해야 할)을 호출
   */
  void callSchedule(void);

  /**
   * @brief 컴포넌트 정보 출력 (디버깅용)
   */
  void showComponent();

  bool chronosCheckPoint(const std::string comp_name, int comp_idx,
                         std::function<bool(void)> logic);
};

/**
 * @brief PCAP을 이용하여 패킷 수신 시 등록된 컴포넌트를 호출
 * @param chain
 * @param pcap_resource
 * @param thread_num
 * @param forever
 * @return true
 * @return false
 */
bool pcapModuleCall(ComponentChain &chain, const char *pcap_resource,
                    int thread_num, bool forever);

/**
 * @brief 서버-클라이언트 구조로 클라이언트로 부터 수신 시 컴포넌트를
 * 호출
 * @param chain
 * @param thread_num
 * @param port
 * @return true
 * @return false
 */
bool serverModuleCall(ComponentChain &chain, int thread_num, int port);

/**
 * @brief 서버-클라이언트 구조로 서버로 접속 시 컴포넌트를 호출
 *
 * @param chain
 * @param thread_num
 * @param port
 * @param terminate_tv
 * @param monitor_tv
 * @return true
 * @return false
 */
bool clientModuleCall(ComponentChain &chain, int thread_num, int port,
                      struct timeval *terminate_tv = NULL,
                      struct timeval *monitor_tv = NULL);
}; // namespace soso

#endif