#include "../picuno/picuno.h"
byte buf[95];
void setup(){
    Serial_begin(115200);
    Serial2_begin(115200);
    
}
void loop(){
    if(Serial2_available())
        Serial_putc(Serial2_read());
    if(Serial_available())
        Serial2_putc(Serial_read());    
}
