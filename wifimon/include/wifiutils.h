#ifndef __WIFI_UTILES_H__
#define __WIFI_UTILES_H__

#include <stddef.h>
#include <stdint.h>

uint8_t freq_to_channel(int freq);
bool is_nullmac(const uint8_t *mac);
bool is_broadcastmac(const uint8_t *mac);
bool maccmp(const uint8_t *mac1, const uint8_t *mac2);
void maccpy(uint8_t *to, const uint8_t *from);
size_t ssidcpy(char *to, const char *from, size_t len);

#endif