#include "chronos.h"

using namespace soso;
using namespace std;

Chronos::Chronos(const string &desc, ChronosStack *cstack) {
  _desc = desc;
  _cstack = cstack;
  gettimeofday(&_begin, NULL);
  timerclear(&_end);
  timerclear(&_diff);
}

Chronos::~Chronos() {
  if (++_once_destructure == 1) {
    end();
    if (_cstack != nullptr) {
      _cstack->addChronos(*this);
    }
  }
}

const string Chronos::desc() { //
  return _desc;
}

void Chronos::begin() { //
  gettimeofday(&_begin, NULL);
}

void Chronos::end() { //
  gettimeofday(&_end, NULL);
  timersub(&_end, &_begin, &_diff);

#if 0
  string e = to_string(_end.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_end.tv_usec));

  string b = to_string(_begin.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_begin.tv_usec));

  string d = to_string(_diff.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_diff.tv_usec));
  cout << "chronos\n";
  cout << "\t" << e << endl;
  cout << "\t" << b << endl;
  cout << "\t" << d << endl;
#endif
}

struct timeval &Chronos::diff() { //
  return _diff;
}

ChronosStack::ChronosStack(const string &title, size_t max_point) { //
  _add_count = 0;
  _title = title;
  _max_point = max_point;
  if (_max_point == 0) {
    _max_point = 1;
  }
  timerclear(&_total);
  timerclear(&_avg);
}

ChronosStack::ChronosStack(const ChronosStack &cs) : _lock() {
  _title = cs._title;
  _points = cs._points;
  _max_point = cs._max_point;
  _total = cs._total;
  _avg = cs._avg;
  _add_count = cs._add_count;
}

size_t ChronosStack::checkedCount() { //
  return _add_count;
}

size_t ChronosStack::pointSize() { //
  return _points.size();
}

void ChronosStack::addChronos(Chronos &chronos) {
  const lock_guard<mutex> lock(_lock);

  _add_count++;

  if (_points.size() >= _max_point) {
    auto &last = _points.back();
    if (timercmp(&last.diff(), &chronos.diff(), <)) {
      _points.push_front(chronos);
      _points.pop_back();
    }
  } else {
    _points.push_back(chronos);
  }

  // sort desc
  _points.sort([](Chronos &a, Chronos &b) {
    if (a.diff().tv_sec == b.diff().tv_sec) {
      return (b.diff().tv_usec < a.diff().tv_usec);
    }
    return (b.diff().tv_sec < a.diff().tv_sec);
  });

  /// total
  timeradd(&_total, &chronos.diff(), &_total);

  /// avg
  uint64_t avg_time =
      (((_total.tv_sec * 1000000) + _total.tv_usec) / _add_count);
  _avg.tv_sec = avg_time / 1000000;
  _avg.tv_usec = avg_time % 1000000;
}

const string ChronosStack::toString() {

  string results = "";
  string total;
  string avg;
  size_t i;

  total = to_string(_total.tv_sec) + "." + //
          fmt::format("{0:0>7}", to_string(_total.tv_usec));

  avg = to_string(_avg.tv_sec) + "." + //
        fmt::format("{0:0>7}", to_string(_avg.tv_usec));

  results += fmt::format("[{}] total:{}, avg:{}\n", _title, total, avg);
  i = 0;

  {
    const lock_guard<mutex> lock(_lock);

    for (auto &p : _points) {
      results += "(" + to_string(++i) + ") " +      //
                 p.desc() + ": " +                  //
                 to_string(p.diff().tv_sec) + "." + //
                 fmt::format("{0:0>7}", to_string(p.diff().tv_usec)) + "\n";
    }
  }
  return results;
}

void ChronosStack::monitoring(function<void(ChronosStack &)> f) {
  const lock_guard<mutex> lock(_lock);
  f(*this);
}

list<Chronos> &ChronosStack::getPoints() { //
  return _points;
}

const string ChronosStack::title() { //
  return _title;
}

struct timeval ChronosStack::getTotal() {
  return _total;
}

struct timeval ChronosStack::getAvg() {
  return _avg;
}