#include "../picuno/picuno.h"
#include "ESP8266-01.h"

byte buf[32];
byte test_data[]={"+CIFSR:APIP,\"7.0.0.0\"\r\n+CIFSR:APMAC,\"20:46:39:00:00:00\"\r\n+CIFSR:STAIP,\"0.0.0.0\"\r\n+CIFSR:STAMAC,\"5c:cf:7f:b2:41:f2\"\r\n\r\nOK"};
//byte str_mode[3]="2";
byte index_start,index_end;
byte ipa[20],ips[20];
byte line1[20],line2[20];
byte mode=2;

const char* ssid="OPPO F9";
const char* password="01234567";

void setup(){
    byte dd[32];
    Serial_begin(115200);
    Serial2_begin(115200);
    

    sprintf(buf,"Restart status:%d",esp8266_restart());
    Serial_println(buf);
    
//    memset(dd,0,sizeof(dd));
//    Serial_print(test_data);
//    index_start = FindChar(0,test_data,','); 
//    index_end = FindChar(0,test_data + index_start,'\r') - 1;
//    sprintf(buf,"\r\n\r\nindex start=%d \r\nindex end=%d",index_start, index_end);
//    Serial_println(buf);
//    strncpy(dd,test_data + index_start, index_end);
//    Serial_println(dd);
//    
//    index_start = InStr(0,test_data,"STAIP");
//    index_start = index_start + (strlen("STAIP"));
//    index_end = FindChar(0,test_data + index_start, '\r')-1;
//    sprintf(buf, "\r\nindex start=%d\r\n\r\nindex end=%d\r\n", index_start,index_end);
//    Serial_println(buf);
//    strncpy(dd,test_data + index_start, index_end);
//    Serial_println(dd);

//    if(_sATCWJAP(ssid,password))
//        Serial_println("WiFi Connected");
//    else
//        Serial_println("Error");
//    Serial_println(_getLocalIP());

//    sprintf(buf,"Status:%d",_sATCWMODE(mode));
//    Serial_println(buf);
//    if(_qATCWMODE(&mode)){
//    sprintf(buf,"mode:%d",mode);
//    Serial_println(buf);
//    }
//    sprintf(buf,"Version:%s",_eATGMR());
//    Serial_println(buf);
    Serial_clear();
    Serial2_clear();
}
void loop(){
    if(Serial2_available())
        Serial_putc(Serial2_read());
    if(Serial_available())
        Serial2_putc(Serial_read());     
}
