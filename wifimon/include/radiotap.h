#ifndef __RADIO_TAP_H__
#define __RADIO_TAP_H__

#include <stdint.h>

typedef struct radiotap_t {
  uint8_t it_version;
  uint8_t it_pad;
  uint16_t it_len;
  uint32_t it_present;
} __attribute__((packed)) radiotap_t;

#endif