#include "TimeDiffer.hpp"
#include <iostream>
#include <unistd.h>

using namespace std;

void delay(int ms) { usleep(ms); }

int main(void) {
  TimeDiffer td("Test");
  td.setMaxPointSize(5);

  td.checkpoint("1) check #100 ");
  delay(100);

  td.checkpoint("2) check #1000");
  delay(1000);

  td.checkpoint("3) check #300 ");
  delay(300);

  td.checkpoint("4) check #50  ");
  delay(50);

  td.checkpoint("5) check #500 ");
  delay(500);

  td.checkpoint("6) check #700 ");
  delay(700);

  td.checkpoint("7) check #2000");
  delay(2000);

  td.checkpoint();

  cout << td.toString() << "\n";

  TimeCheckPoint p1("p1");
  usleep(500);
  TimeCheckPoint p2("p2");

  cout << p1.checkTimeGT(p2) << "\n";
  cout << p1.checkTimeLT(p2) << "\n";

  return 0;
}
