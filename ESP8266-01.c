#include "ESP8266-01.h"
//Global Variable
byte _rx_data[UART2_RX_MAX];
struct arrayData esp;

/*=============================================================================
 Top Level function
 =============================================================================*/
bool esp8266_restart(void){
	
	if (_eATRST()) {
		delay(2000);
		timerSet(ESP_TIMER, TIMEOUT);
		while (timerBusy(ESP_TIMER)) {
			if (_eAT()) delay(1500); /* Waiting for stable */
            if(_eATE(0)) {
                delay(100);
                return true;
            }	
		}
	}
	return false;    
}

bool esp8266_kick(void){
    return _eAT();
}
/*=============================================================================
Middle Upper level function 
 =============================================================================*/
bool _setOprToStation(void){
    uint8_t mode;
    if (!_qATCWMODE(&mode)) return false;
	if (mode == 3) return true;
	else {
		if (_sATCWMODE(1) && esp8266_restart()) {
			return true;
			} 
		}
return false;     
}

bool _setOprToSoftAP(void){
    uint8_t mode;
    if (!_qATCWMODE(&mode)) return false;
	if (mode == 3) return true;
	else {
		if (_sATCWMODE(2) && esp8266_restart()) {
			return true;
			} 
		}
return false;    
}
bool _setOprToStationSoftAP(void){
    uint8_t mode;
    if (!_qATCWMODE(&mode)) return false;
	if (mode == 3) return true;
	else {
		if (_sATCWMODE(2) && esp8266_restart()) {
			return true;
			} 
		}
return false;      
}

bool _enableMUX(void){
    return _sATCIPMUX(1);
}
bool _disableMUX(void){
    return _sATCIPMUX(0);
}

bool _joinAP(const char* ssid, const char* pwd){
    return _sATCWJAP(ssid,pwd);
}

bool _leaveAP(void){
    return _eATCWQAP();
}
/*=============================================================================
 Middle lower level function
 =============================================================================*/
bool _eATE(uint8_t enable){
    Serial2_clear();
    if(enable){
        Serial2_println("ATE1");
    }
    else{
        Serial2_println("ATE0");
    }
    return _recvFind("OK",1000);
}

bool _eAT(void){
    Serial2_clear();    
    Serial2_println("AT");
    return _recvFind("OK",1000);
}

bool _eATRST(void){
    Serial2_clear();
    Serial2_println("AT+RST");
    return _recvFind("OK",3000);
}

const char* _eATGMR(void){
    byte _rx_data[UART2_RX_MAX];
    
    Serial2_clear();
    Serial2_println("AT+GMR");
    timerSet(ESP_TIMER,TIMEOUT);
    while(!Serial2_ready() && timerBusy(ESP_TIMER));
    if(Serial2_ready()){
        Serial2_readBytes(_rx_data,Serial2_available());
    }
    if(InStr(0,_rx_data,"OK"))
        return _rx_data;
    else return "NULL";
}

bool _sATCWJAP(const char* ssid, const char* pwd){

    Serial2_clear();    
    Serial2_print("AT+CWJAP=\"");
    Serial2_print(ssid);
    Serial2_print("\",\"");
    Serial2_print(pwd);
    Serial2_println("\"");
    
    if(_recvString("OK",10000)) return true;
    return false;

}

bool _eATCWQAP(void){
    Serial2_clear();
    Serial2_println("AT+CWQAP");
    return _recvFind("OK",2000);
}

const char* _getLocalIP(void){
    byte dd[64];
    byte ip1[20];
    byte ip2[20];
    byte mode;
    byte _sPos;
    byte _ePos;
    
    if(!_qATCWMODE(&mode))
        return;
    
    Serial2_clear();
    memset(_rx_data,0,sizeof(_rx_data));
    memset(dd,0,sizeof(dd));
    memset(ip1,0,sizeof(ip1));
    memset(ip2,0,sizeof(ip2));
    
    Serial2_println("AT+CIFSR");
    timerSet(ESP_TIMER,TIMEOUT);
    if(_recvString("OK",1000)){
        if(mode == 1 || mode == 2)   //STA Mode
        {
         /*
          DECODE RESPONSE
          MODE == 1
          +CIFSR:STAIP,"79.80.80.79"
          +CIFSR:STAMAC,"5c:cf:7f:b2:41:f2"
          MODE==2
           +CIFSR:APIP,"7.0.0.0"
           +CIFSR:APMAC,"20:46:39:00:00:00"
          */   
            _sPos = FindChar(0,_rx_data,',');
            _ePos = FindChar (0,_rx_data,'\r') - 1;
            strncpy(ip1,_rx_data+_sPos,_ePos);  
            
            if(mode == 1)
                sprintf(dd,"STAIP:%s",ip1);
            
            else
                sprintf(dd,"APIP:%s",ip1);
        }

        else if (mode == 3) //Both Mode
        {

            /*
             DECODE RESPONSE
             +CIFSR:APIP,"7.0.0.0"
            +CIFSR:APMAC,"20:46:39:00:00:00"
            +CIFSR:STAIP,"79.80.80.79"
            +CIFSR:STAMAC,"5c:cf:7f:b2:41:f2"
             */

            _sPos = FindChar(0,_rx_data,',');
            _ePos = FindChar(0, _rx_data + _sPos, '\r') - 1;
            strncpy(ip1,_rx_data+ _sPos, _ePos);
            
            _sPos = InStr(0,_rx_data,"STAIP");
            _sPos = _sPos + (strlen("STAIP"));
            _ePos = FindChar(0, _rx_data + _sPos, '\r') - 1;
            strncpy (ip2, _rx_data+ _sPos, _ePos);
            
            sprintf(dd,"APIP:%s\r\nSTAIP:%s",ip1,ip2);
        }
        return dd;
    }
    return "No IP";
}

bool _sATCIPMUX(uint8_t mode){
    Serial2_clear();
    memset(esp.buf,0,sizeof(esp.buf));
    sprintf(esp.buf,"AT+CIPMUX=%d",mode);
    Serial2_println(esp.buf);
    if(_recvFind("OK",3000))
        return true;
    return false;
}

bool _sATCWMODE(uint8_t mode){
    byte _str[15];
//    byte _rx_data[32];
    
    //Clear RX buffer
    Serial2_clear();
    
    //Prepare Command
    sprintf(_str,"AT+CWMODE=%d",mode);
    
    //Sending AT+CWMODE=mode
    Serial2_println(_str);
    
    //Set timeout to 5000ms
    timerSet(ESP_TIMER,5000);
    
    //wait until data receive within 5000ms
    while(!Serial2_ready() && timerBusy (ESP_TIMER));
    
    //if data coming, copy it to _rx_data
    if(Serial2_ready()){
        Serial2_readBytes(_rx_data,Serial2_available());
    }
    //otherwise, return false
    else
        return false;
    
    //if _rx_data contain "OK" or "no change", then return true
    if(InStr(0,_rx_data,"OK") || InStr(0,_rx_data,"no change"))
        return true;
    
    //otherwise return false
    return false;
}

bool _sATCIPSTARTMultiple(uint8_t mux_id, const char* type, const char* addr, uint16_t port){
    Serial2_clear();
    memset(esp.buf,0,sizeof(esp.buf));
    sprintf(esp.buf,"AT+CIPSTART=%d,\"%s\",\"%s\",%d",mux_id,type,addr,port);
    Serial2_println(esp.buf);
    if(_recvFind("ERROR",5000))
        return false;
    return true;
}

bool _sATCIPSERVER(uint8_t mode,uint16_t port){
    Serial2_clear();
    memset(esp.buf,0,sizeof(esp.buf));
    if(mode){   //mode 1 to open
        sprintf(esp.buf,"AT+CIPSERVER=1,%d",port);
        Serial2_println(esp.buf);
        if(_recvFind("OK",TIMEOUT))
            return true;
    }
    else{
        Serial2_println("AT+CIPSERVER=0");
        if(_recvFind("OK",TIMEOUT))
            return true;
    }
    return false;
}

bool _sATCIPSTO(uint16_t timeout){  
    Serial2_clear();
    memset(esp.buf,0,sizeof(esp.buf));
    sprintf(esp.buf,"AT+AT+CIPSTO=%d",timeout);
    Serial2_println(esp.buf);
    if(_recvFind("OK",TIMEOUT)) return true;
    return false;
}
/*=============================================================================
 Low level function
 =============================================================================*/

/*
 AT+CWMODE?
 +CWMOMDE:2
 
 OK
 * 1=Sta
 * 2=AP
 * 3=Both
 */
bool _qATCWMODE(uint8_t *mode){
    byte _mode[1];
    byte _rx_data[32];
    byte _index;
    
    //Make sure mode is NOT zero;
    if (!mode){
        return false;
    }
    
    //Clear RX buffer
    Serial2_clear();
    
    //Send Query AT+CWMODE?
    Serial2_println("AT+CWMODE?");
    
    //Set timeout = 5000ms
    timerSet(ESP_TIMER,5000);
    
    //Wait until receive data within 5000ms
    while(!Serial2_ready() && timerBusy(ESP_TIMER));
    
    //If data was received, then Decode
    if(Serial2_ready()){
        Serial2_readBytes(_rx_data,Serial2_available());
        _index = FindChar(0,_rx_data,':');
        _mode[0] = _rx_data[_index];
    }
    
    //Data not received within 5000ms, then return false
    else
        return false;
    
    //Update mode value
    *mode = StrToUInt(_mode);
    
    //Return true if clean process
    return true;
}


bool _recvFind(const char* target, uint16_t timeout) {   
    uint8_t so_far = 0;
    uint8_t received;
    uint8_t length = strlen(target);
    uint8_t rtn_value = 0;
    bool continue_loop = true;    
    timerSet(ESP_TIMER,timeout);
    do {
        if(Serial2_available())
        {
            timerSet(ESP_TIMER,timeout);
            received = Serial2_read();
            if (received == target[so_far]) {
                so_far++;                
            } else {
                so_far = 0;
                rtn_value = false;
            }
            if(so_far == length){
             continue_loop = false;  
             rtn_value = true;
            } 
        }
    } while (continue_loop == true && timerBusy(ESP_TIMER));
    if(continue_loop == false && timerBusy(ESP_TIMER))
    {
        timerSet(ESP_TIMER,1000);
        while(timerBusy(ESP_TIMER));
    }
    return rtn_value;
}

bool _recvString (const char* target, uint16_t timeout){
//    byte _rx_data[UART2_RX_MAX];
    byte _index;
    byte a;
    bool rtnVal;
    //reload timer   
    timerSet(ESP_TIMER,TIMEOUT);
    _index = 0;
    rtnVal = false;
    while(timerBusy(ESP_TIMER)){
         while(Serial2_available()>0){
             a = Serial2_read();
             if(a == '\0') continue;
             _rx_data[_index++] = a;
         }
         if(InStr(0,_rx_data,target)){
             rtnVal = true;
             break;
         }
    }
    return rtnVal;
}
