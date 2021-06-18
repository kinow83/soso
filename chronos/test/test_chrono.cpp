#include "chrono.h"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace soso;

int main(void) {
  ChronoStack cstack("Test", 4);
  Chrono chronos[] = {
      Chrono("factor#1"),  //
      Chrono("factor#2"),  //
      Chrono("factor#3"),  //
      Chrono("factor#4"),  //
      Chrono("factor#5"),  //
      Chrono("factor#6"),  //
      Chrono("factor#7"),  //
      Chrono("factor#8"),  //
      Chrono("factor#9"),  //
      Chrono("factor#10"), //
  };

  for (auto &c : chronos) {
    c.begin();
    usleep(10);
    c.end();

    cstack.addChrono(c);
  }

  cout << cstack.toString();
  cout << cstack.checkedCount() << endl;
  cout << cstack.pointSize() << endl;

  return 0;
}
