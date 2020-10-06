#include "ESP8266-01.h"

//Middle level function
bool eAT(void){
    Serial2_clear();    
    Serial2_println("AT");
    return _recvFind("OK",1000);
}

bool eATRST(void){
    Serial2_clear();
    Serial2_println("AT+RST");
    return _recvFind("OK",3000);
}

bool qATCWMODE(uint8_t *mode){
    byte _mode[1];
    byte _rx_data[32];
    byte _index;
    
    Serial2_clear();
    Serial2_println("AT+CWMODE?");
    timerSet(ESP_TIMER,5000);
    while(!Serial2_ready() && timerBusy(ESP_TIMER));
    if(Serial2_ready()){
        Serial2_readBytes(_rx_data,Serial2_available());
        _index = FindChar(0,_rx_data,':');
        _mode[0] = _rx_data[_index];
    }
    else
        return false;
    *mode = StrToUInt(_mode);
    return true;
}
//Low level function
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
char* _recvString (const char* target, uint16_t timeout){
    byte rx_data[32];
    //reload timer   
    timerSet(ESP_TIMER,timeout);
    while(timerBusy(ESP_TIMER)){
        if(Serial2_ready()){            
            Serial2_readBytes(rx_data,UART2_RX_MAX);
            if(0,rx_data,target)break;
        }
    }
    return rx_data;
}
