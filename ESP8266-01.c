#include "ESP8266-01.h"

bool _recvFind(char* target, uint16_t timeout) {   
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
