#ifndef __ESP8266_H__
#define __ESP8266_H__
#include "../picuno/picuno.h"

#define ESP_TIMER 5

bool _recvFind(const char* target, uint16_t timeout);
char* _recvString (const char* target, uint16_t timeout);
bool _eAT(void);
bool _eATRST(void);
const char* _eATGMR(void);
bool _sATCWJAP(const char* ssid, const char* pwd);
bool _sATCWMODE(uint8_t mode);
bool _qATCWMODE(uint8_t *mode);

#endif /* #ifndef __ESP8266_H__ */