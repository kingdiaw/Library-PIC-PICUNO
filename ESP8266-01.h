#ifndef __ESP8266_H__
#define __ESP8266_H__
#include "../picuno/picuno.h"

#define ESP_TIMER 5
#define TIMEOUT 10000

struct arrayData {
   uint8_t  array[UART2_RX_MAX];
   uint8_t  length;
   uint8_t  buf[32];
};

//TOP LEVEL FUNCTION
bool esp8266_restart(void);
bool esp8266_kick(void);

//MIDDLE UPPER LEVEL FUNCTION
bool _setOprToStation(void);
bool _setOprToStationSoftAP(void);
bool _setOprToStationSoftAP(void);
bool _enableMUX(void);
bool _disableMUX(void);
bool _joinAP(const char* ssid, const char* pwd);
bool _leaveAP(void);

/*=============================================================================
 //MIDDLE LOWER LEVEL FUNCTION
 =============================================================================*/

//Basic AT Command
bool _eAT(void);
bool _eATRST(void);
const char* _eATGMR(void);
bool _eATGSLP(void);    //Enter Deep-Sleep mode
bool _eATE(uint8_t enable);
bool _eATSLEEP(void);   //Configure sleep mode
bool _sATWAKEUPGPIO(void);  //Configure GPIO to wake up

//WiFi AT Command
bool _sATCWMODE(uint8_t mode);
bool _qATCWMODE(uint8_t *mode);
bool _sATCWJAP(const char* ssid, const char* pwd);
bool _eATCWQAP(void);
bool _sATCIPSTA(const char* ip);    //Set IP address of the ESP8266 Station
bool _sATCIPAP (const char* ip);    //SetIP address of ESP8266 SoftAP

//TCP/IP AT Command for ESP8266
bool _eATCIPSTATUS(void);   // Get the connection status
bool _eATCIPSTART(void);    //Establishes TCP connection, UDP transmission or SSL
bool _eATCIPSEND(const char* data); //Send data
bool _eATCIPCLOSE(void);    //Close TCP/UDP/SSL Connection
const char* _getLocalIP(void);
bool _sATCIPMUX(uint8_t mode);
bool _eATCIPSERVER(uint8_t chn,uint16_t port);  //Delete/create TCP Server


//LOW LEVEL FUNCTION
bool _recvFind(const char* target, uint16_t timeout);
bool _recvString (const char* target, uint16_t timeout);




#endif /* #ifndef __ESP8266_H__ */
