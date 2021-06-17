#include "chronos.hpp"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace soso;

void delay(int ms) { usleep(ms); }

int main(void) {
  Chronos chronos("Test");
  chronos.setMaxCheckPoint(5);

  chronos.checkpoint("1) check #100 ");
  delay(100);

  chronos.checkpoint("2) check #1000");
  delay(1000);

  chronos.checkpoint("3) check #300 ");
  delay(300);

  chronos.checkpoint("4) check #50  ");
  delay(50);

  chronos.checkpoint("5) check #500 ");
  delay(500);

  chronos.checkpoint("6) check #700 ");
  delay(700);

  chronos.checkpoint("7) check #2000");
  delay(2000);

  chronos.checkpoint();

  cout << chronos.toString() << "\n";

  ChronosCheckPoint p1("p1");
  usleep(500);
  ChronosCheckPoint p2("p2");

  cout << p1.checkTimeGT(p2) << "\n";
  cout << p1.checkTimeLT(p2) << "\n";

  return 0;
}
