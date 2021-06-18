#include "wifiutils.h"
#include <fmt/format.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int wifi_channel_freq_table[][2] = {
    {1, 2412},  {2, 2417},  {3, 2422},  {4, 2427},  {5, 2432},
    {6, 2437},  {7, 2442},  {8, 2447},  {9, 2452},  {10, 2457},
    {11, 2462}, {12, 2467}, {13, 2472}, {14, 2484},
};

uint8_t freq_to_channel(int freq) {
  if (freq == 2484) {
    return 14;
  }
  return ((freq - 2412) / 5) + 1;
}

bool is_nullmac(const uint8_t *mac) {
  static uint8_t nullmac[6] = {0, 0, 0, 0, 0, 0};
  return (memcmp(mac, nullmac, sizeof(nullmac)) == 0);
}

bool is_broadcastmac(const uint8_t *mac) {
  static uint8_t bmac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  return (memcmp(mac, bmac, sizeof(bmac)) == 0);
}

bool maccmp(const uint8_t *mac1, const uint8_t *mac2) {
  return (memcmp(mac1, mac2, 6) == 0);
}

void maccpy(uint8_t *to, const uint8_t *from) {
  //
  memcpy(to, from, 6);
}

size_t ssidcpy(char *to, const char *from, size_t len) {
  size_t real_len;
  size_t ssid_len;

  real_len = strlen(from);
  ssid_len = (real_len < len) ? real_len : len;
  if (ssid_len > 32) {
    ssid_len = 32;
  }
  memcpy(to, from, ssid_len);
  to[ssid_len] = '\0';
  return ssid_len;
}

#if 0
int main() {

  /*
    for (int i = 0; i < 14; i++) {
      printf("%d\n", freq_to_channel(wifi_channel_freq_table[i][1]));
    }
  */

  char to[33];
  const char *from = "1234567890";
  ssidcpy(to, from, 32);
  printf("%s\n", to);

#define libfmtmac "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}"
#define libfmtst(st) st[0], st[1], st[2], st[3], st[4], st[5]
  uint8_t a[6] = {
      0,
  };
  std::string s = fmt::format("aaaaaa: " libfmtmac, libfmtst(a));
  printf("%s\n", s.c_str());
}
#endif