#ifndef __SOSO_KAIROS_H__
#define __SOSO_KAIROS_H__

#include <assert.h>
#include <bits/stdc++.h>
#include <fmt/format.h>
#include <list>
#include <mutex>
#include <stdbool.h>
#include <stdint.h>
#include <string>
#include <sys/time.h>
#include <vector>

namespace soso {

class KairosStack;

/**
 * @brief 소요 시간 측정 class
 */
class Kairos {
private:
  /// 소요 시간 측정 설명
  std::string _desc;
  /// 측정 시점 시각
  struct timeval _begin;
  struct timeval _end;
  /// 소요 시간 측정 값
  struct timeval _diff;
  ///
  class KairosStack *_kstack;
  size_t _once_destructure = 0;

public:
  Kairos(const std::string &desc, KairosStack *kstack = nullptr);
  ~Kairos();
  void begin();
  void end();
  struct timeval &diff();
  const std::string desc();
};

/**
 * @brief 소요 시간 측정 요소 관리
 *
 */
class KairosStack {
private:
  std::string _title;
  std::list<Kairos> _points;
  std::mutex _lock;
  size_t _max_point;

  struct timeval _total;
  struct timeval _avg;
  size_t _add_count;

public:
  /**
   * @brief KairoStack 생성자
   * @param title
   * @param max_point 시간 측정 값 최대 저장 갯수
   */
  KairosStack(const std::string &title, size_t max_point = 0);
  KairosStack(const KairosStack &cs);

  void incMaxPoint(size_t inc = 1) { _max_point += inc; }
  void decMaxPoint(size_t inc = 1) { _max_point -= inc; }

  /**
   * @brief 시간 측정 값을 stack에 추가 (thread-safe)
   * @param Kairo
   */
  void addKairos(Kairos &&kairos) { //
    addKairos(kairos);
  }
  void addKairos(Kairos &kairos);

  const std::string toString();
  size_t checkedCount();
  size_t pointSize();
  void monitoring(std::function<void(KairosStack &)> f);
  std::list<Kairos> &getPoints();
  const std::string title();
  struct timeval getTotal();
  struct timeval getAvg();
};

}; // namespace soso

#endif