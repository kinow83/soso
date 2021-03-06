#ifndef __SOSO_COMPONENT_H__
#define __SOSO_COMPONENT_H__

#include "kairos.h"
#include "request.h"
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
  std::string _name;

protected:
  /// No operation (Next 컴포넌트 호출)
  bool noop() { return true; }

public:
  /**
   * @brief Component 생성자
   * @param name
   */
  Component(const std::string &name) : _name(name) { //
  }
  /**
   * @brief 컴포넌트 이름 반환
   * @return const std::string
   */
  const std::string getName() { return _name; }

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
  virtual bool prepare(Request *request) = 0;

  /**
   * @brief 컴포넌트 수행 가상 멤버 함수
   * @param request
   * @return true
   * @return false
   */
  virtual bool process(Request *request) = 0;

  /**
   * @brief 컴포넌트 후처리 가상 멤버 함수
   * @param request
   * @return true
   * @return false
   */
  virtual bool post(Request *request) = 0;

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
  std::vector<KairosStack> _kstack;
  bool _trace_kairos = true;

  /// 컴포넌트 구성요청 체인 리스트
  std::vector<std::shared_ptr<Component>> _chains;

public:
  ComponentChain();

  virtual ~ComponentChain();

  size_t componentSize() { return _chains.size(); }

  const std::string report() {
    std::string results = "[ComponentChain::report]\n";

    for (KairosStack &kairos : _kstack) {
      results += kairos.toString();
    }
    return results;
  }

  void kairosMonitor(COMPONENT_IDX comp_idx,
                     std::function<void(KairosStack &)> f);

  /**
   * @brief 컴포넌트 추가
   * @param component
   */
  void registers(std::shared_ptr<Component> component);

  /**
   * @brief 컴포넌트 접근자
   * @param name
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
  void callComponent(Request *request);

  /**
   * @brief 컴포넌트의 schedule(단일 스레드로 동작해야 할)을 호출
   */
  void callSchedule(void);

  bool kairosCheck(const std::string comp_name, int comp_idx,
                   std::function<bool(void)> logic);
};

}; // namespace soso

#endif