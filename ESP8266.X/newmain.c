#include "picuno.h"

const uint8_t* ssid="Kdw";
const uint8_t* pass="aa112233";
const uint8_t* ip="smtp.elasticemail.com";
const uint8_t* port="2525";
const uint8_t AT[4]="AT+";
const uint8_t userID64[] ="ZXNwbWN1OUBnbWFpbC5jb20=";
const uint8_t pass64[]="ZDY2ZWVlZDAtYjQyNC00MDUwLThiNmEtMDQ2NzQwMjQwYWM4";
const uint8_t mailFrom[]="espmcu9@gmail.com";
const uint8_t sendTo[]="kingdiawehsut@gmail.com";
const uint8_t subject[]="Mail From ESP8266";
uint8_t mailContent[]="Testing Berjaya";

uint8_t buf[50]=0;
uint8_t temp[50];
uint8_t chkBuf[50];
uint8_t cc;

void setup(){
 Serial2_begin(9600);  
Serial_begin(9600);
pinMode(PIN_RD1,INPUT);
Serial2_println("ATE0");
Serial_println("Press PB1 to Start");
while(digitalRead(PIN_RD1));
}

void esp8266_print(uint8_t* data){
    Serial2_println(data);
    Serial_println(data);
}
//
//inline uint16_t _esp8266_waitFor(unsigned char *string) {
//    unsigned char so_far = 0;
//    unsigned char received;
//    uint16_t counter = 0;
//    do {
//        if(Serial2_available())
//        received = Serial2_read();
//        counter++;
//        if (received == string[so_far]) {
//            so_far++;
//        } else {
//            so_far = 0;
//        }
//    } while (string[so_far] != 0);
//    return counter;
//}
//
//inline unsigned char _esp8266_waitResponse(void) {
//    unsigned char so_far[6] = {0,0,0,0,0,0};
//    unsigned const char lengths[6] = {2,5,4,9,6,6};
//    unsigned const char* strings[6] = {"OK", "ready", "FAIL", "no change", "Linked", "Unlink"};
//    unsigned const char responses[6] = {ESP8266_OK, ESP8266_READY, ESP8266_FAIL, ESP8266_NOCHANGE, ESP8266_LINKED, ESP8266_UNLINK};
//    unsigned char received;
//    unsigned char response;
//    bool continue_loop = true;
//    while (continue_loop) {
//        if(Serial2_available())
//        received = Serial2_read();
//        for (unsigned char i = 0; i < 6; i++) {
//            if (strings[i][so_far[i]] == received) {
//                so_far[i]++;
//                if (so_far[i] == lengths[i]) {
//                    response = responses[i];
//                    continue_loop = false;
//                }
//            } else {
//                so_far[i] = 0;
//            }
//        }
//    }
//    return response;
//}
void esp8266_chk(void)
{
    uint8_t loopHere=1;
    static uint8_t f=0,fOld=1;
    
    if(fOld =! f){
        chkBuf[0]=0;
        timerSet(0,3000);
        fOld = f;
        loopHere=1;
    }
    do{
        if(Serial2_available()){
            timerSet(0,3000);
            Serial_putc(Serial2_read());
            fOld=1;
            f=0; 
            }
        if(timerUp(0)){
            loopHere = 0;
        }
        
    }while(loopHere==1);
}

void cipSend(uint8_t* dd){
    uint8_t len[3];
    sprintf(len,"%u",strlen(dd)+2);
    temp[0]=0;
    strcpy(temp,AT);
    strcat(temp,"CIPSEND=4,");
    strcat(temp,len);
    esp8266_print(temp);
    timerSet(0,1000);
    while((Serial2_read() != '>') && timerBusy(0));
    delay(100);
    esp8266_print(dd);
}

void loop(){
    while(1){
        if(Serial_available())
        {
            Serial2_putc(Serial_read());
        }
        if(Serial2_available())
        {
            Serial_putc(Serial2_read());
        }        
    }
    
esp8266_print("AT");
esp8266_chk();

//AT+CWMODE=3
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CWMODE=3");
esp8266_print(buf);
esp8266_chk();

//AT+CWJAP="Kdw","aa112233"
buf[0];
strcpy(buf,AT);
strcat(buf,"CWJAP=\"");
strcat(buf,ssid);
strcat(buf,"\",\"");
strcat(buf,pass);
strcat(buf,"\"");
esp8266_print(buf);
esp8266_chk();

//AT+CIPMUX=1
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CIPMUX=1");
esp8266_print(buf);
esp8266_chk();

//AT+CIPSERVER=1,80
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CIPSERVER=1,80");
esp8266_print(buf);
esp8266_chk();

//AT+CIPSTART=4,"TCP","smtp.elasticemail.com",2525
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CIPSTART=4,\"TCP\",\"");
strcat(buf,ip);
strcat(buf,"\",");
strcat(buf,port);
esp8266_print(buf);
esp8266_chk();

cipSend("EHLO 54.38.226.140");
esp8266_chk();

cipSend("AUTH LOGIN");
esp8266_chk();

cipSend(userID64);
esp8266_chk();

cipSend(pass64);
esp8266_chk();

//cipSend("MAIL FROM:<mailFrom>");
buf[0]=0;
strcat(buf,"MAIL FROM:<");
strcat(buf,mailFrom);
strcat(buf,">");
cipSend(buf);
esp8266_chk();

//cipSend("RCPT TO:<sendTo>");
buf[0]=0;
strcat(buf,"RCPT TO:<");
strcat(buf,sendTo);
strcat(buf,">");
cipSend(buf);
esp8266_chk();

cipSend("DATA");
esp8266_chk();

//cipSend("SUBJECT:Mail From ESP8266");
buf[0]=0;
strcat(buf,"SUBJECT:");
strcat(buf,subject);
cipSend(buf);
esp8266_chk();

cipSend("");
esp8266_chk();

//cipSend(mailContent);
buf[0]=0;
strcat(buf,mailContent);
cipSend(buf);
esp8266_chk();

cipSend(".");
esp8266_chk();

cipSend("QUIT");
esp8266_chk();

while(1);
}

