#ifndef __ESP8266_H__
#define __ESP8266_H__
#include "../picuno/picuno.h"

#define ESP_TIMER 5
#define TIMEOUT 10000

//TOP LEVEL FUNCTION
bool esp8266_restart(void);

//MIDDLE UPPER LEVEL FUNCTION
<<<<<<< HEAD
bool _setOprToStation(void);
bool _setOprToSoftAP(void);
=======
>>>>>>> 220f24ee38ce9f9268890340f78b99dad1f71bd2
bool _setOprToStationSoftAP(void);

//MIDDLE LOWER LEVEL FUNCTION
bool _eATE(uint8_t enable);
bool _eAT(void);
bool _eATRST(void);
const char* _eATGMR(void);
bool _sATCWJAP(const char* ssid, const char* pwd);
bool _eATCWQAP(void);
bool _sATCWMODE(uint8_t mode);
bool _qATCWMODE(uint8_t *mode);
const char* _getLocalIP(void);

//LOW LEVEL FUNCTION
bool _recvFind(const char* target, uint16_t timeout);
bool _recvString (const char* target, uint16_t timeout);




#endif /* #ifndef __ESP8266_H__ */