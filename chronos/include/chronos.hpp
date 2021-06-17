#ifndef __SOSO_CHRONOS_H__
#define __SOSO_CHRONOS_H__

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
struct ChronosCheckPoint {
  /// 소요 시간 측정 설명
  std::string _desc;
  /// 측정 시점 시각
  struct timeval _checktime;
  /// 소요 시간 측정 값
  struct timeval _difftime;

  /**
   * @brief TimeCheckPoint 생성자
   * @param desc
   */
  ChronosCheckPoint(const std::string &desc = "") {
    this->_desc = desc;
    gettimeofday(&this->_checktime, NULL);
    timerclear(&this->_difftime);
  }
  /**
   * @brief 설명 설정
   * @param desc
   */
  void setDesc(const std::string &desc) { this->_desc = desc; }
  /**
   * @brief 측정 시간 업데이트
   */
  void updateCheckTime() { gettimeofday(&this->_checktime, NULL); }

  /**
   * @brief difftime 시간이 checkpoint 보다 큰 값인지 확인 (difftime 시간 크기
   * 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool diffTimeGT(const ChronosCheckPoint &checkpoint) {
    return timercmp(&this->_difftime, &checkpoint._difftime, >) ? true : false;
  }
  /**
   * @brief difftime 시간이 checkpoint 보다 작은 값인지 확인 (difftime 시간
   * 크기 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool diffTimeLT(const ChronosCheckPoint &checkpoint) {
    return timercmp(&this->_difftime, &checkpoint._difftime, <) ? true : false;
  }
  /**
   * @brief checktime 시간이 checkpoint 보다 큰 값인지 확인 (checktime 시간 크기
   * 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool checkTimeGT(const ChronosCheckPoint &checkpoint) {
    return timercmp(&this->_checktime, &checkpoint._checktime, >) ? true
                                                                  : false;
  }
  /**
   * @brief checktime 시간이 checkpoint 보다 작은 값인지 확인 (checktime 시간
   * 크기 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool checkTimeLT(const ChronosCheckPoint &checkpoint) {
    return timercmp(&this->_checktime, &checkpoint._checktime, <) ? true
                                                                  : false;
  }
};

/**
 * @brief 프로그램 시간 소요 시간 측정\n
 * checkpoint()를 호출할 때마다 소요된 시간 측정 값을 저장
 */
class Chronos {
private:
  /// 시간 측정 키워드
  std::string _title;
  /// 시간 측정 값 배열
  std::list<ChronosCheckPoint> _points;
  /// 시간 측정 값 최대 저장 갯수
  size_t _max_check_point = 100;
  /// checkpoint 호출 횟수
  size_t _checked_count = 0;
  /// 평균 소요 값
  ChronosCheckPoint _avg;
  /// 전체 소요 시간
  ChronosCheckPoint _total;
  /// 오랜 시간 소요 목록
  std::list<ChronosCheckPoint> _long_time_points;
  /// 오랜 시간 소요 목록 갯수
  size_t _max_long_time_point = 5;

public:
  /**
   * @brief TimeDiffer 생성자
   */
  Chronos() {}

  /**
   * @brief TimeDiffer 생성자
   * @param title
   */
  Chronos(const std::string &title) {
    this->_title = title;
    _avg.setDesc(title + "-average");
    _total.setDesc(title + "-total");
  }

  /**
   * @brief 오랜 시간 소요 목록 수 지정
   * @param max
   */
  void setMaxLongTimeCheckPoint(size_t max) { //
    this->_max_long_time_point = max;
  }

  /**
   * @brief 시간 측정 값 최대 목록 수 지정
   * @param max
   */
  void setMaxCheckPoint(size_t max) { //
    this->_max_check_point = max;
  }

  /**
   * @brief 시간 측정 키워드 지정
   * @param title
   */
  void setTitle(const std::string &title) {
    this->_title = title;
    _avg.setDesc(title + "-average");
    _total.setDesc(title + "-total");
  }

  /**
   * @brief 측정 종료
   */
  struct timeval checkpoint() {
    return checkpoint("", false);
  }

  /**
   * @brief 호출 시 마다 현재 시각과 이전 시간과의 차이를 기록함
   * @param desc
   * @return struct timeval
   */
  struct timeval checkpoint(const std::string &desc, bool continuous = true) {
    ChronosCheckPoint checkpoint(desc);

    if (_points.size() == 0) {
      _points.push_back(checkpoint);
      _checked_count++;
    } else {
      ChronosCheckPoint &last = _points.back();
      timersub(&checkpoint._checktime, &last._checktime, &last._difftime);
      if (continuous) {
        _points.push_back(checkpoint);
        _checked_count++;
      }

      /// long time point
      if (_long_time_points.size() == 0) {
        _long_time_points.push_back(last);
      } else {
        ChronosCheckPoint &min = _long_time_points.back();

        auto cmp = ([](ChronosCheckPoint &a, ChronosCheckPoint &b) {
          if (a._difftime.tv_sec == b._difftime.tv_sec) {
            return (b._difftime.tv_usec < a._difftime.tv_usec);
          }
          return (b._difftime.tv_sec < a._difftime.tv_sec);
        });

        if (_long_time_points.size() < _max_long_time_point) {
          _long_time_points.push_back(last);
          _long_time_points.sort(cmp);
        } else {
          if (min.diffTimeLT(last)) {
            _long_time_points.push_back(last);
            _long_time_points.sort(cmp);

            size_t i = 1;
            for (auto it = _long_time_points.begin(); //
                 it != _long_time_points.end();       //
                 it++) {
              i++;
              if (i > _max_long_time_point) {
                _long_time_points.erase(it, _long_time_points.end());
                break;
              }
            }
          }
        }
      }

      /// total elapsed
      _total.updateCheckTime();
      timeradd(&_total._difftime, &last._difftime, &_total._difftime);

      /// avg elapsed
      _avg.updateCheckTime();
      uint64_t avg_time =
          ((_total._difftime.tv_sec * 1000000) + _total._difftime.tv_usec) /
          _checked_count;
      _avg._difftime.tv_sec = avg_time / 1000000;
      _avg._difftime.tv_usec = avg_time % 1000000;

      ///
      if (_points.size() > _max_check_point) {
        _points.pop_front();
      }
    }
    return checkpoint._difftime;
  }

  /**
   * @brief 최종 시간 측정 값 출력
   * @return const std::string
   */
  const std::string toString() {
    std::string results = "";
    size_t i;

    results +=
        "[" + _total._desc + "] " +                     //
        std::to_string(_total._difftime.tv_sec) + "." + //
        fmt::format("{0:0>7}", std::to_string(_total._difftime.tv_usec)) + "\n";
    results += "\n[" + _avg._desc + "] " +                   //
               std::to_string(_avg._difftime.tv_sec) + "." + //
               fmt::format("{0:0>7}", std::to_string(_avg._difftime.tv_usec)) +
               "\n";

    results += "\n[check point stack]\n";
    i = 0;
    for (auto p : _points) {
      results += "(" + std::to_string(++i) + ") ";
      results += "[" + this->_title + "] " + p._desc + ": " + //
                 std::to_string(p._difftime.tv_sec) + "." +   //
                 fmt::format("{0:0>7}", std::to_string(p._difftime.tv_usec)) +
                 "\n";
    }
    results += "\n[long time check point stack]\n";
    i = 0;
    for (auto p : _long_time_points) {
      results += "(" + std::to_string(++i) + ") ";
      results += "[" + this->_title + "] " + p._desc + ": " + //
                 std::to_string(p._difftime.tv_sec) + "." +   //
                 fmt::format("{0:0>7}", std::to_string(p._difftime.tv_usec)) +
                 "\n";
    }
    return results;
  }
};

}; // namespace soso

#endif