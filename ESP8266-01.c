#include "ESP8266-01.h"

/*=============================================================================
 Middle level function
 =============================================================================*/

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
    timerSet(ESP_TIMER,5000);
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

bool _sATCWMODE(uint8_t mode){
    byte _str[15];
    byte _rx_data[32];
    
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
    byte _rx_data[UART2_RX_MAX];
    //reload timer   
    timerSet(ESP_TIMER,timeout);
    while(timerBusy(ESP_TIMER)){
     if(Serial2_ready()){
         Serial2_readBytes(_rx_data,Serial2_available());
         if(InStr(0,_rx_data,target))
         break;   
    }
     return true;
    }
    return false;
}