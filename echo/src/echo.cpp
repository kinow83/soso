#include "echo.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

using namespace soso;

/*
<List of colors for prompt and Bash>
txtblk='\e[0;30m' # Black - Regular
txtred='\e[0;31m' # Red
txtgrn='\e[0;32m' # Green13
txtylw='\e[0;33m' # Yellow
txtblu='\e[0;34m' # Blue
txtpur='\e[0;35m' # Purple
txtcyn='\e[0;36m' # Cyan
txtwht='\e[0;37m' # White
bldblk='\e[1;30m' # Black - Bold
bldred='\e[1;31m' # Red
bldgrn='\e[1;32m' # Green
bldylw='\e[1;33m' # Yellow
bldblu='\e[1;34m' # Blue
bldpur='\e[1;35m' # Purple
bldcyn='\e[1;36m' # Cyan
bldwht='\e[1;37m' # White
unkblk='\e[4;30m' # Black - Underline
undred='\e[4;31m' # Red
undgrn='\e[4;32m' # Green
undylw='\e[4;33m' # Yellow
undblu='\e[4;34m' # Blue
undpur='\e[4;35m' # Purple
undcyn='\e[4;36m' # Cyan
undwht='\e[4;37m' # White
bakblk='\e[40m'   # Black - Background
bakred='\e[41m'   # Red
bakgrn='\e[42m'   # Green
bakylw='\e[43m'   #log_level White
txtrst='\e[0m'    # Text Reset
 */

#ifndef LOG_NO_COLOR
#define VTC_BLUE "\x1b[34m"   //!< Colour following text blue.
#define VTC_RED "\x1b[31m"    //!< Colour following text red.
#define VTC_GREEN "\x1b[32m"  //!< Colour following text creen.
#define VTC_YELLOW "\x1b[33m" //!< Colour following text yellow.
#define VTC_BOLD "\x1b[1m"    //!< Embolden following text.
#define VTC_WHITE "\x1b[37m"  //!< Colour following text white.

#define VTC_RESET "\x1b[0m" //!< Reset terminal text to default style/colour.
#endif

static const char *LOG_STR[] = {
    "FATAL", //
    "ERROR", //
    "INFO ", //
    "DEBUG", //
    "TRACE",
};

#define LOG_BUFSIZE 10240

Echo::Echo(const char *filename, int lv) {
  setLevel(lv);
  assert((filename != NULL));
  this->_filename = filename;
}

Echo::~Echo() {}

void Echo::setLevel(int lv) {
  //
  this->_log_level = lv;
}

void Echo::consoleLog(int lv, const char *fmt, va_list ap, uint8_t *hex,
                      size_t len, const char *color, bool bold) {
  struct tm t_now;
  time_t now;
  char strtime[256];
  char buf[LOG_BUFSIZE];
  const char *lv_str;
  int n, i;
  FILE *fp = NULL;
  static char init_env = 0;

  if (_log_level >= lv) {
    if (_filename) {
      fp = fopen(_filename, "a+");
      assert(fp != NULL);
    } else {
      fp = stdout;
    }

    if (init_env == 0) {
      // os default file buffering.
      setvbuf(fp, (char *)NULL, _IOLBF, (size_t)0);
      init_env = 1;
    }

    lv_str = LOG_STR[lv];
    now = time(NULL);
    localtime_r(&now, &t_now);

    strftime(strtime, sizeof(strtime), "%H:%M:%S", &t_now);
    n = vsnprintf(buf, sizeof(buf), fmt, ap);

    if (hex) {
      for (i = 0; i < (int)len; i++) {
        n += snprintf(buf + n, sizeof(buf) - n, " %02x", hex[i]);
      }
    }

    if (bold == true) {
      fprintf(fp, "%s", VTC_BOLD);
    }
    if (hex) {
      fprintf(fp, "%s%s: %s: (len:%ld) %s\n", color, strtime, lv_str, len, buf);
    } else {
      fprintf(fp, "%s%s: %s: %s\n", color, strtime, lv_str, buf);
    }
    fprintf(fp, "%s", VTC_RESET);

    if (_exit_on_fatal && (lv == L_FATAL)) {
      exit(1);
    }

    if (fp != stdout) {
      fclose(fp);
    }
  }
}

/*
 * normal
 */
void Echo::f(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_FATAL, fmt, ap, NULL, 0, VTC_YELLOW, false);
  va_end(ap);
}

void Echo::e(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_ERROR, fmt, ap, NULL, 0, VTC_RED, false);
  va_end(ap);
}

void Echo::i(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_INFO, fmt, ap, NULL, 0, VTC_GREEN, false);
  va_end(ap);
}

void Echo::d(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_DEBUG, fmt, ap, NULL, 0, VTC_WHITE, false);
  va_end(ap);
}

void Echo::t(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_TRACE, fmt, ap, NULL, 0, VTC_WHITE, false);
  va_end(ap);
}

/*
 * bold
 */
void Echo::F(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_FATAL, fmt, ap, NULL, 0, VTC_YELLOW, true);
  va_end(ap);
}

void Echo::E(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_ERROR, fmt, ap, NULL, 0, VTC_RED, true);
  va_end(ap);
}

void Echo::I(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_INFO, fmt, ap, NULL, 0, VTC_GREEN, true);
  va_end(ap);
}

void Echo::D(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  va_end(ap);
  consoleLog(L_DEBUG, fmt, ap, NULL, 0, VTC_WHITE, true);
}

void Echo::T(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  va_end(ap);
  consoleLog(L_TRACE, fmt, ap, NULL, 0, VTC_WHITE, true);
}

/*
 * hex
 */

void Echo::fx(uint8_t *hex, size_t len, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_FATAL, fmt, ap, hex, len, VTC_YELLOW, false);
  va_end(ap);
}

void Echo::ex(uint8_t *hex, size_t len, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_ERROR, fmt, ap, hex, len, VTC_RED, false);
  va_end(ap);
}

void Echo::ix(uint8_t *hex, size_t len, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_INFO, fmt, ap, hex, len, VTC_GREEN, false);
  va_end(ap);
}

void Echo::dx(uint8_t *hex, size_t len, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_DEBUG, fmt, ap, hex, len, VTC_WHITE, false);
  va_end(ap);
}

void Echo::tx(uint8_t *hex, size_t len, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  consoleLog(L_TRACE, fmt, ap, hex, len, VTC_YELLOW, false);
  va_end(ap);
}

Echo echo;

void default_echo_level(int lv) {
  //
  echo.setLevel(lv);
}