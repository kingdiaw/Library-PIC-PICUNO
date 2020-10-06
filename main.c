#include "../picuno/picuno.h"
#include "ESP8266-01.h"

byte buf[32];
byte rx_data[32];
//byte str_mode[3]="2";
byte index;
byte mode;

void setup(){
    Serial_begin(115200);
    Serial2_begin(115200);
    sprintf(buf,"RETURN:%d",qATCWMODE(&mode));
    Serial_println(buf);
    sprintf(buf,"mode:%d",mode);
    Serial_println(buf);
}
void loop(){
    
}
