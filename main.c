#include "../picuno/picuno.h"
#include "ESP8266-01.h"

byte buf[32];
byte rx_data[32];
//byte str_mode[3]="2";
byte index;
byte mode=2;

const char* ssid="OPPO F9";
const char* password="01234567";

void setup(){
    Serial_begin(115200);
    Serial2_begin(115200);
    if(_eAT())
        Serial_println("Kick");
    if(_sATCWJAP(ssid,password))
        Serial_println("WiFi Connected");
    Serial_println("Error");
    
//    sprintf(buf,"Status:%d",_sATCWMODE(mode));
//    Serial_println(buf);
//    if(_qATCWMODE(&mode)){
//    sprintf(buf,"mode:%d",mode);
//    Serial_println(buf);
//    }
}
void loop(){
    if(Serial2_available())
        Serial_putc(Serial2_read());
    if(Serial_available())
        Serial2_putc(Serial_read());     
}
