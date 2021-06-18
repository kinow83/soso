#ifndef __SOSO_H_STRUCT_H__
#define __SOSO_H_STRUCT_H__

#include <stdint.h>

namespace soso {

typedef struct phy_info_t {
  uint8_t channel;
  int8_t signal;
  int8_t noise;
  uint16_t rate;
  uint16_t ch_flag;
} phy_info_t;

typedef struct tag_param_t {
  uint8_t type;
  uint8_t length;
  uint8_t value[0];
} tag_param_t;

}; // namespace soso
#endif