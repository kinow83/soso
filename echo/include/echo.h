#ifndef __SOSO_ECHO_H__
#define __SOSO_ECHO_H__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

namespace soso {

#define L_FATAL 0
#define L_ERROR 1
#define L_INFO 2
#define L_DEBUG 3
#define L_TRACE 4
#define L_ALL (L_TRACE + 1)

#define macfmt "%02x:%02x:%02x:%02x:%02x:%02x"
#define MACFMT "%02X:%02X:%02X:%02X:%02X:%02X"

/**
 * @brief 로그 출력 (화면 또는 특정 파일)
 */
class Echo {
private:
  /// 로그레벨\n
  /// TRACE > DEBUG > INFO > ERROR > FATAL
  int _log_level = L_DEBUG;
  /// 로그 출력 파일명
  const char *_filename = NULL;
  /// fatal 로그 호출 시 프로그램 종료 여부
  bool _exit_on_fatal = false;
  /// 화면으로 로그 출력
  void consoleLog(int lv, const char *fmt, va_list ap, uint8_t *hex, size_t len,
                  const char *color, bool bold);

public:
  /**
   * @brief 로그 레벨 설정
   * @param lv L_TRACE > L_DEBUG > L_INFO > L_ERROR > L_FATAL
   */
  void setLevel(int lv);
  /**
   * @brief fatal 로그 호출 시 프로그램을 종료하지 않음
   */
  void exitOnFatal() { _exit_on_fatal = true; }
  /**
   * @brief Echo 생성자
   * @param lv
   */
  Echo(int lv = L_ALL) : _log_level(lv) {}
  Echo(const char *filename, int lv = L_ALL);
  ~Echo();
  /**
   * @brief fatal 일반 로그 출력 \n
   * (주의! 로그 출력 후 exit 1로 프로그램 종료 됨)
   * @param fmt
   * @param ...
   */
  void f(const char *fmt, ...);
  /**
   * @brief error 일반 로그 출력
   * @param fmt
   * @param ...
   */
  void e(const char *fmt, ...);
  /**
   * @brief info 일반 로그 출력
   * @param fmt
   * @param ...
   */
  void i(const char *fmt, ...);
  /**
   * @brief debug 일반 로그 출력
   * @param fmt
   * @param ...
   */
  void d(const char *fmt, ...);
  /**
   * @brief trace 일반 로그 출력
   * @param fmt
   * @param ...
   */
  void t(const char *fmt, ...);

  /**
   * @brief fatal bold 로그 출력 \n
   * (주의! 로그 출력 후 exit 1로 프로그램 종료 됨)
   * @param fmt
   * @param ...
   */
  void F(const char *fmt, ...);
  /**
   * @brief error bold 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void E(const char *fmt, ...);
  /**
   * @brief info bold 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void I(const char *fmt, ...);
  /**
   * @brief debug bold 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void D(const char *fmt, ...);
  /**
   * @brief trace bold 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void T(const char *fmt, ...);

  /**
   * @brief fatal hex 로그 출력 \n
   * (주의! 로그 출력 후 exit 1로 프로그램 종료 됨)
   * @param fmt
   * @param ...
   */
  void fx(uint8_t *hex, size_t len, const char *fmt, ...);
  /**
   * @brief error hex 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void ex(uint8_t *hex, size_t len, const char *fmt, ...);
  /**
   * @brief info hex 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void ix(uint8_t *hex, size_t len, const char *fmt, ...);
  /**
   * @brief debug hex 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void dx(uint8_t *hex, size_t len, const char *fmt, ...);
  /**
   * @brief trace hex 로그 출력 \n
   * @param fmt
   * @param ...
   */
  void tx(uint8_t *hex, size_t len, const char *fmt, ...);
};

/**
 * @brief 기본 echo 객체에 대한 로그 레벨 설정
 * @param lv
 */
void default_echo_level(int lv);

}; // namespace soso

/**
 * @brief echo 기본 echo 모듈 \n
 * 주의! static 변수이므로 오브젝트 마다 echo 객체가 생성됨
 */
extern soso::Echo echo;

#endif