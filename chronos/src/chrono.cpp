#include "chrono.h"

using namespace soso;

Chrono::Chrono(const std::string &desc) {
  _desc = desc;
  gettimeofday(&_begin, NULL);
  timerclear(&_end);
  timerclear(&_diff);
}

const std::string Chrono::desc() { //
  return _desc;
}

void Chrono::begin() { //
  gettimeofday(&_begin, NULL);
}

void Chrono::end() { //
  gettimeofday(&_end, NULL);
  timersub(&_end, &_begin, &_diff);
}

struct timeval &Chrono::diff() { //
  return _diff;
}

ChronoStack::ChronoStack(const std::string &title, size_t max_point) { //
  _title = title;
  _max_point = max_point;
  if (_max_point == 0) {
    _max_point = 1;
  }
  timerclear(&_total);
  timerclear(&_avg);
}

size_t ChronoStack::checkedCount() { //
  return _add_count;
}

size_t ChronoStack::pointSize() { //
  return _points.size();
}

void ChronoStack::addChrono(Chrono &chrono) {
  const std::lock_guard<std::mutex> lock(_lock);

  _add_count++;

  if (_points.size() > _max_point) {
    auto &last = _points.back();
    if (timercmp(&last.diff(), &chrono.diff(), <)) {
      _points.push_front(chrono);
      _points.pop_back();
    }
  } else {
    _points.push_back(chrono);
  }

  // sort desc
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
}

const std::string ChronoStack::toString() {
  const std::lock_guard<std::mutex> lock(_lock);

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
               p.desc() + ": " +                       //
               std::to_string(p.diff().tv_sec) + "." + //
               fmt::format("{0:0>7}", std::to_string(p.diff().tv_usec)) + "\n";
  }
  return results;
}