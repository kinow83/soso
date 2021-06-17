#ifndef __SOSO_CHRONO_H__
#define __SOSO_CHRONO_H__

#include <assert.h>
#include <bits/stdc++.h>
#include <fmt/format.h>
#include <list>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <vector>

namespace soso {
/**
 * @brief 소요 시간 측정 class
 *
 */
class Chrono {
private:
  /// 소요 시간 측정 설명
  std::string _desc;
  /// 측정 시점 시각
  struct timeval _begin;
  struct timeval _end;
  /// 소요 시간 측정 값
  struct timeval _diff;

public:
  Chrono(const std::string &desc) {
    _desc = desc;
    gettimeofday(&_begin, NULL);
    timerclear(&_end);
    timerclear(&_diff);
  }

  void begin() { //
    gettimeofday(&_begin, NULL);
  }

  void end() { //
    gettimeofday(&_end, NULL);
    timersub(&_end, &_begin, &_diff);
  }

  struct timeval &diff() {
    return _diff;
  }
};

class ChronoStack {
private:
  std::string _title;
  std::list<Chrono> _points;
  size_t _max_point = 100;

  struct timeval _total;
  struct timeval _avg;
  size_t _add_count = 0;

public:
  ChronoStack(const std::string &title, size_t max_point) { //
    _title = title;
    _max_point = max_point;
    timerclear(&_total);
    timerclear(&_avg);
  }

  void addChrono(Chrono &chrono) {
    _add_count++;
    //////////////////////////////////
    /// TODO: 추후 성능 개선
    //////////////////////////////////
    _points.push_back(chrono);
    _points.sort([](Chrono &a, Chrono &b) {
      if (a.diff().tv_sec == b.diff().tv_sec) {
        return (b.diff().tv_usec < a.diff().tv_usec);
      }
      return (b.diff().tv_sec < a.diff().tv_sec);
    });

    /// total
    timeradd(&_total, &chrono.diff(), &_total);

    /// avg
    uint64_t avg_time =
        (((_total.tv_sec * 1000000) + _total.tv_usec) / _add_count);
    _avg.tv_sec = avg_time / 1000000;
    _avg.tv_usec = avg_time % 1000000;

    if (_points.size() > _max_point) {
      _points.pop_back();
    }
  }

  const std::string toString() {
    std::string results = "";
    std::string total;
    std::string avg;
    size_t i;

    total = std::to_string(_total.tv_sec) + "." + //
            fmt::format("{0:0>7}", std::to_string(_total.tv_usec));

    avg = std::to_string(_avg.tv_sec) + "." + //
          fmt::format("{0:0>7}", std::to_string(_avg.tv_usec));

    results += fmt::format("[{}] total:{}, avg:{}\n", _title, total, avg);
    i = 0;
    for (auto p : _points) {
      results += "(" + std::to_string(++i) + ") " +      //
                 std::to_string(p.diff().tv_sec) + "." + //
                 fmt::format("{0:0>7}", std::to_string(p.diff().tv_usec)) +
                 "\n";
    }
    return results;
  }
};

}; // namespace soso

#endif