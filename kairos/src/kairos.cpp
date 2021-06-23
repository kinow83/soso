#include "kairos.h"

using namespace soso;
using namespace std;

Kairos::Kairos(const string &desc, KairosStack *kstack) {
  _desc = desc;
  _kstack = kstack;
  gettimeofday(&_begin, NULL);
  timerclear(&_end);
  timerclear(&_diff);
}

Kairos::~Kairos() {
  if (++_once_destructure == 1) {
    end();
    if (_kstack != nullptr) {
      _kstack->addKairos(*this);
    }
  }
}

const string Kairos::desc() { //
  return _desc;
}

void Kairos::begin() { //
  gettimeofday(&_begin, NULL);
}

void Kairos::end() { //
  gettimeofday(&_end, NULL);
  timersub(&_end, &_begin, &_diff);

#if 0
  string e = to_string(_end.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_end.tv_usec));

  string b = to_string(_begin.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_begin.tv_usec));

  string d = to_string(_diff.tv_sec) + "." + //
             fmt::format("{0:0>7}", to_string(_diff.tv_usec));
  cout << "Kairos\n";
  cout << "\t" << e << endl;
  cout << "\t" << b << endl;
  cout << "\t" << d << endl;
#endif
}

struct timeval &Kairos::diff() { //
  return _diff;
}

KairosStack::KairosStack(const string &title, size_t max_point) { //
  _add_count = 0;
  _title = title;
  _max_point = max_point;
  timerclear(&_total);
  timerclear(&_avg);
}

KairosStack::KairosStack(const KairosStack &kstack) : _lock() {
  _title = kstack._title;
  _points = kstack._points;
  _max_point = kstack._max_point;
  _total = kstack._total;
  _avg = kstack._avg;
  _add_count = kstack._add_count;
}

size_t KairosStack::checkedCount() { //
  return _add_count;
}

size_t KairosStack::pointSize() { //
  return _points.size();
}

void KairosStack::addKairos(Kairos &kairos) {
  const lock_guard<mutex> lock(_lock);

  _add_count++;

  if (_points.size() >= _max_point) {
    auto &last = _points.back();
    if (timercmp(&last.diff(), &kairos.diff(), <)) {
      _points.push_front(kairos);
      _points.pop_back();
    }
  } else {
    _points.push_back(kairos);
  }

  // sort desc
  _points.sort([](Kairos &a, Kairos &b) {
    if (a.diff().tv_sec == b.diff().tv_sec) {
      return (b.diff().tv_usec < a.diff().tv_usec);
    }
    return (b.diff().tv_sec < a.diff().tv_sec);
  });

  /// total
  timeradd(&_total, &kairos.diff(), &_total);

  /// avg
  uint64_t avg_time =
      (((_total.tv_sec * 1000000) + _total.tv_usec) / _add_count);
  _avg.tv_sec = avg_time / 1000000;
  _avg.tv_usec = avg_time % 1000000;
}

const string KairosStack::toString() {

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

void KairosStack::monitoring(function<void(KairosStack &)> f) {
  const lock_guard<mutex> lock(_lock);
  f(*this);
}

list<Kairos> &KairosStack::getPoints() { //
  return _points;
}

const string KairosStack::title() { //
  return _title;
}

struct timeval KairosStack::getTotal() {
  return _total;
}

struct timeval KairosStack::getAvg() {
  return _avg;
}