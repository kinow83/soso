#ifndef __TIME_DIFFER_H__
#define __TIME_DIFFER_H__

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

/**
 * @brief 소요 시간 측정 class
 *
 */
struct TimeCheckPoint {
  /// 소요 시간 측정 설명
  std::string desc;
  /// 측정 시점 시각
  struct timeval checktime;
  /// 소요 시간 측정 값
  struct timeval difftime;

  /**
   * @brief TimeCheckPoint 생성자
   * @param desc
   */
  TimeCheckPoint(const std::string &desc = "") {
    this->desc = desc;
    gettimeofday(&this->checktime, NULL);
    timerclear(&this->difftime);
  }
  /**
   * @brief 설명 설정
   * @param desc
   */
  void setDesc(const std::string &desc) { this->desc = desc; }
  /**
   * @brief 측정 시간 업데이트
   */
  void updateCheckTime() { gettimeofday(&this->checktime, NULL); }

  /**
   * @brief difftime 시간이 checkpoint 보다 큰 값인지 확인 (difftime 시간 크기
   * 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool diffTimeGT(const TimeCheckPoint &checkpoint) {
    return timercmp(&this->difftime, &checkpoint.difftime, >) ? true : false;
  }
  /**
   * @brief difftime 시간이 checkpoint 보다 작은 값인지 확인 (difftime 시간
   * 크기 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool diffTimeLT(const TimeCheckPoint &checkpoint) {
    return timercmp(&this->difftime, &checkpoint.difftime, <) ? true : false;
  }
  /**
   * @brief checktime 시간이 checkpoint 보다 큰 값인지 확인 (checktime 시간 크기
   * 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool checkTimeGT(const TimeCheckPoint &checkpoint) {
    return timercmp(&this->checktime, &checkpoint.checktime, >) ? true : false;
  }
  /**
   * @brief checktime 시간이 checkpoint 보다 작은 값인지 확인 (checktime 시간
   * 크기 비교)
   * @param checkpoint
   * @return true
   * @return false
   */
  bool checkTimeLT(const TimeCheckPoint &checkpoint) {
    return timercmp(&this->checktime, &checkpoint.checktime, <) ? true : false;
  }
};

/**
 * @brief 프로그램 시간 소요 시간 측정\n
 * checkpoint()를 호출할 때마다 소요된 시간 측정 값을 저장
 */
class TimeDiffer {
private:
  /// 시간 측정 키워드
  std::string title;
  /// 시간 측정 값 배열
  std::list<TimeCheckPoint> points;
  /// 시간 측정 값 최대 저장 갯수
  size_t maxPointSize = 100;
  /// checkpoint 호출 횟수
  size_t checkedCount = 0;
  /// 평균 소요 값
  TimeCheckPoint avg;
  /// 전체 소요 시간
  TimeCheckPoint total;
  /// 오랜 시간 소요 목록
  std::list<TimeCheckPoint> longTimePoints;
  /// 오랜 시간 소요 목록 갯수
  size_t maxLongTimePointSize = 5;

public:
  /**
   * @brief TimeDiffer 생성자
   */
  TimeDiffer() {}

  /**
   * @brief TimeDiffer 생성자
   * @param title
   */
  TimeDiffer(const std::string &title) {
    this->title = title;
    avg.setDesc(title + "-average");
    total.setDesc(title + "-total");
  }

  /**
   * @brief 오랜 시간 소요 목록 수 지정
   * @param max
   */
  void setMaxLongTimePointsSize(size_t max) { //
    this->maxLongTimePointSize = max;
  }

  /**
   * @brief 시간 측정 값 최대 목록 수 지정
   * @param max
   */
  void setMaxPointSize(size_t max) { //
    this->maxPointSize = max;
  }

  /**
   * @brief 시간 측정 키워드 지정
   * @param title
   */
  void setTitle(const std::string &title) {
    this->title = title;
    avg.setDesc(title + "-average");
    total.setDesc(title + "-total");
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
    TimeCheckPoint checkpoint(desc);

    if (points.size() == 0) {
      points.push_back(checkpoint);
      checkedCount++;
    } else {
      TimeCheckPoint &last = points.back();
      timersub(&checkpoint.checktime, &last.checktime, &last.difftime);
      if (continuous) {
        points.push_back(checkpoint);
        checkedCount++;
      }

      /// long time point
      if (longTimePoints.size() == 0) {
        longTimePoints.push_back(last);
      } else {
        TimeCheckPoint &min = longTimePoints.back();

        auto cmp = ([](TimeCheckPoint &a, TimeCheckPoint &b) {
          if (a.difftime.tv_sec == b.difftime.tv_sec) {
            return (b.difftime.tv_usec < a.difftime.tv_usec);
          }
          return (b.difftime.tv_sec < a.difftime.tv_sec);
        });

        if (longTimePoints.size() < maxLongTimePointSize) {
          longTimePoints.push_back(last);
          longTimePoints.sort(cmp);
        } else {
          if (min.diffTimeLT(last)) {
            longTimePoints.push_back(last);
            longTimePoints.sort(cmp);

            size_t i = 1;
            for (auto it = longTimePoints.begin(); //
                 it != longTimePoints.end();       //
                 it++) {
              i++;
              if (i > maxLongTimePointSize) {
                longTimePoints.erase(it, longTimePoints.end());
                break;
              }
            }
          }
        }
      }

      /// total elapsed
      total.updateCheckTime();
      timeradd(&total.difftime, &last.difftime, &total.difftime);

      /// avg elapsed
      avg.updateCheckTime();
      uint64_t avg_time =
          ((total.difftime.tv_sec * 1000000) + total.difftime.tv_usec) /
          checkedCount;
      avg.difftime.tv_sec = avg_time / 1000000;
      avg.difftime.tv_usec = avg_time % 1000000;

      ///
      if (points.size() > maxPointSize) {
        points.pop_front();
      }
    }
    return checkpoint.difftime;
  }

  /**
   * @brief 최종 시간 측정 값 출력
   * @return const std::string
   */
  const std::string toString() {
    std::string results = "";
    size_t i;

    results += "[" + total.desc + "] " +                     //
               std::to_string(total.difftime.tv_sec) + "." + //
               fmt::format("{0:0>7}", std::to_string(total.difftime.tv_usec)) +
               "\n";
    results += "\n[" + avg.desc + "] " +                   //
               std::to_string(avg.difftime.tv_sec) + "." + //
               fmt::format("{0:0>7}", std::to_string(avg.difftime.tv_usec)) +
               "\n";

    results += "\n[check point stack]\n";
    i = 0;
    for (auto p : points) {
      results += "(" + std::to_string(++i) + ") ";
      results += "[" + this->title + "] " + p.desc + ": " + //
                 std::to_string(p.difftime.tv_sec) + "." +  //
                 fmt::format("{0:0>7}", std::to_string(p.difftime.tv_usec)) +
                 "\n";
    }
    results += "\n[long time check point stack]\n";
    i = 0;
    for (auto p : longTimePoints) {
      results += "(" + std::to_string(++i) + ") ";
      results += "[" + this->title + "] " + p.desc + ": " + //
                 std::to_string(p.difftime.tv_sec) + "." +  //
                 fmt::format("{0:0>7}", std::to_string(p.difftime.tv_usec)) +
                 "\n";
    }
    return results;
  }
};

#endif