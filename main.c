#include "../picuno/picuno.h"
#include "ESP8266-01.h"

byte buf[32];
void setup(){
    Serial_begin(115200);
    Serial2_begin(115200);
    sprintf(buf,"%d",_recvFind("OK",3000));
    Serial_println(buf);
}
void loop(){
    
}
