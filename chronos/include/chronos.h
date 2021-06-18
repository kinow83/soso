#ifndef __SOSO_CHRONO_H__
#define __SOSO_CHRONO_H__

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

/**
 * @brief 소요 시간 측정 class
 */
class Chronos {
private:
  /// 소요 시간 측정 설명
  std::string _desc;
  /// 측정 시점 시각
  struct timeval _begin;
  struct timeval _end;
  /// 소요 시간 측정 값
  struct timeval _diff;

public:
  Chronos(const std::string &desc);
  void begin();
  void end();
  struct timeval &diff();
  const std::string desc();
};

/**
 * @brief 소요 시간 측정 요소 관리
 *
 */
class ChronosStack {
private:
  std::string _title;
  std::list<Chronos> _points;
  std::mutex _lock;
  size_t _max_point;

  struct timeval _total;
  struct timeval _avg;
  size_t _add_count = 0;

public:
  /**
   * @brief ChronoStack 생성자
   * @param title
   * @param max_point 시간 측정 값 최대 저장 갯수
   */
  ChronosStack(const std::string &title, size_t max_point);

  /**
   * @brief 시간 측정 값을 stack에 추가 (thread-safe)
   * @param chrono
   */
  void addChronos(Chronos &chronos);
  ChronosStack(const ChronosStack &cs);
  const std::string toString();
  size_t checkedCount();
  size_t pointSize();
};

}; // namespace soso

#endif