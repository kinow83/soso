#include "chronos.h"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace soso;

int main(void) {
  ChronosStack cstack("Test", 4);
  Chronos chronoss[] = {
      Chronos("factor#1"),  //
      Chronos("factor#2"),  //
      Chronos("factor#3"),  //
      Chronos("factor#4"),  //
      Chronos("factor#5"),  //
      Chronos("factor#6"),  //
      Chronos("factor#7"),  //
      Chronos("factor#8"),  //
      Chronos("factor#9"),  //
      Chronos("factor#10"), //
  };

  for (auto &c : chronoss) {
    c.begin();
    usleep(10);
    c.end();

    cstack.addChronos(c);
  }

  cout << cstack.toString();
  cout << cstack.checkedCount() << endl;
  cout << cstack.pointSize() << endl;

  return 0;
}
