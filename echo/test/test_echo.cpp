#include "echo.h"
#include <gtest/gtest.h>

using namespace soso;

void test_show(const char *filename) {
  char cmd[1024];
  snprintf(cmd, sizeof(cmd), "cat %s", filename);
  system(cmd);
}

TEST(Echo, echo_test) {
  const char *filename = "echo.log";
  const char *myname = "test";
  uint8_t hexdump[] = {
      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
      0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
  };

  Echo e(filename);

  e.t("[%s] %s", myname, "normal trace");
  e.d("[%s] %s", myname, "normal debug");
  e.i("[%s] %s", myname, "normal info");
  e.e("[%s] %s", myname, "normal error");
  e.f("[%s] %s", myname, "normal fatal");

  e.T("[%s] %s", myname, "BOLD TRACE");
  e.D("[%s] %s", myname, "BOLD DEBUG");
  e.I("[%s] %s", myname, "BOLD INFO");
  e.E("[%s] %s", myname, "BOLD ERROR");
  e.F("[%s] %s", myname, "BOLD FATAL");

  e.tx(hexdump, sizeof(hexdump), "[%s] %s", myname, "trace hex dump");
  e.dx(hexdump, sizeof(hexdump), "[%s] %s", myname, "debug hex dump");
  e.ix(hexdump, sizeof(hexdump), "[%s] %s", myname, "info hex dump");
  e.ex(hexdump, sizeof(hexdump), "[%s] %s", myname, "error hex dump");
  e.fx(hexdump, sizeof(hexdump), "[%s] %s", myname, "fatal hex dump");

  test_show(filename);
}