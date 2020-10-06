#include "picuno.h"

#define ESP_TIMER   5
const uint8_t* ssid="Kdw";
const uint8_t* pass="aa112233";
const uint8_t* ip="54.38.226.140";
const uint8_t* host="smtp.elasticemail.com";
const uint8_t* port="2525";
const uint8_t AT[4]="AT+";
const uint8_t userID64[] ="ZXNwbWN1OUBnbWFpbC5jb20=";
const uint8_t pass64[]="ZDY2ZWVlZDAtYjQyNC00MDUwLThiNmEtMDQ2NzQwMjQwYWM4";
const uint8_t mailFrom[]="espmcu9@gmail.com";
const uint8_t sendTo[]="king_diaw@yahoo.com";
const uint8_t subject[]="Mail From ESP8266";
uint8_t mailContent[]="Testing Berjaya";

uint8_t buf[50]=0;
uint8_t temp[50];
uint8_t chkBuf[50];
uint8_t do_next=0;

void esp8266_print(uint8_t* data){
    Serial2_println(data);
    Serial_println(data);
}

bool esp8266_waitFor(unsigned char *string) {
    uint8_t so_far = 0;
    uint8_t received;
    uint8_t length = strlen(string);
    uint8_t rtn_value = 0;
    bool continue_loop = true;    
    timerSet(ESP_TIMER,3000);
    do {
        if(Serial2_available())
        {
            timerSet(ESP_TIMER,3000);
            received = Serial2_read();
            if (received == string[so_far]) {
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
void processTerminate(void){
    Serial_println("PROCESS TERMINATE");
    for(;;);
}

void setup(){
Serial2_begin(9600);  
Serial_begin(9600);
pinMode(PIN_RD1,INPUT);
Serial2_println("ATE0");
delay(2000);

//AT    
esp8266_print("AT");
if(esp8266_waitFor("OK")== false)processTerminate();

//AT+CWMODE=3
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CWMODE=3");
esp8266_print(buf);
if(esp8266_waitFor("OK")== false)processTerminate();

//AT+CWJAP?
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CWJAP?");
esp8266_print(buf);
if(esp8266_waitFor("No AP")== true)processTerminate();

//AT+CWJAP?
esp8266_print(buf);
//+CWJAP:"Kdw"
buf[0]=0;
strcat(buf,"+CWJAP:\"");
strcat(buf,ssid);
strcat(buf,"\"");

if(esp8266_waitFor(buf)== false)
{
    //AT+CWJAP="Kdw","aa112233"
    buf[0];
    strcpy(buf,AT);
    strcat(buf,"CWJAP=\"");
    strcat(buf,ssid);
    strcat(buf,"\",\"");
    strcat(buf,pass);
    strcat(buf,"\"");
    esp8266_print(buf);
    if(esp8266_waitFor("WIFI GOT IP")== false)processTerminate();
}

//AT+CIPMUX=1
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CIPMUX=1");
esp8266_print(buf);
if(esp8266_waitFor("OK")== false)processTerminate();

//AT+CIPCLOSE=4
buf[0]=0;
strcpy(buf,AT);
strcat(buf,"CIPCLOSE=4");
esp8266_print(buf);
esp8266_chk();

Serial_println("Press PB1 to Send Email");
while(digitalRead(PIN_RD1));
do_next = 0;
}

void loop(){
//    while(1){
//        if(Serial2_available())
//        {
//            Serial_putc(Serial2_read());
//        }
//        if(Serial_available()){
//            Serial2_putc(Serial_read());
//        }
//    }
switch(do_next)
{
    case 0:
        //AT+CIPSERVER=1,80
        buf[0]=0;
        strcpy(buf,AT);
        strcat(buf,"CIPSERVER=1,80");
        esp8266_print(buf);
        if(esp8266_waitFor("OK"))
            do_next = 1;
        else
            processTerminate();
        break;
    case 1:
        //AT+CIPSTART=4,"TCP","smtp.elasticemail.com",2525
        buf[0]=0;
        strcpy(buf,AT);
        strcat(buf,"CIPSTART=4,\"TCP\",\"");
        strcat(buf,host);
        strcat(buf,"\",");
        strcat(buf,port);
        esp8266_print(buf);
        //esp8266_chk();
        if(esp8266_waitFor("4,CONNECT"))
            do_next = 2;
        else
            processTerminate();        
        break;
    case 2:
        //cipSend("EHLO 54.38.226.140");
        buf[0]=0;
        strcat(buf,"EHLO ");
        strcat(buf,ip);
        cipSend(buf);
        if(esp8266_waitFor("250 OK"))
            do_next = 3;
        else
            processTerminate();        
        break;
    case 3:
        cipSend("AUTH LOGIN");
        if(esp8266_waitFor("334"))
            do_next = 4;
        else
            processTerminate();        
        break;
    case 4:
        cipSend(userID64);
        if(esp8266_waitFor("334"))
            do_next = 5;
        else
            processTerminate();
        break;
    case 5:
        cipSend(pass64);
        if(esp8266_waitFor("235"))
            do_next = 6;
        else
            processTerminate();
        break;
    case 6:
        //cipSend("MAIL FROM:<mailFrom>");
        buf[0]=0;
        strcat(buf,"MAIL FROM:<");
        strcat(buf,mailFrom);
        strcat(buf,">");
        cipSend(buf);
        if(esp8266_waitFor("250 OK"))
            do_next = 7;
        else
            processTerminate();        
        break;
    case 7:
        //cipSend("RCPT TO:<sendTo>");
        buf[0]=0;
        strcat(buf,"RCPT TO:<");
        strcat(buf,sendTo);
        strcat(buf,">");
        cipSend(buf);
        if(esp8266_waitFor("250 OK"))
            do_next = 8;
        else
            processTerminate();        
        break;
    case 8:
        cipSend("DATA");
        if(esp8266_waitFor("354"))
            do_next = 9;
        else
            processTerminate();        
        break;
    case 9:
        //cipSend("SUBJECT:Mail From ESP8266");
        buf[0]=0;
        strcat(buf,"SUBJECT:");
        strcat(buf,subject);
        cipSend(buf);
        if(esp8266_waitFor("SEND OK"))
            do_next = 10;
        else
            processTerminate();        
        break;
    case 10:
        cipSend("");
        if(esp8266_waitFor("SEND OK"))
            do_next = 11;
        else
            processTerminate();        
        break;
    case 11:
        //cipSend(mailContent);
        buf[0]=0;
        strcat(buf,mailContent);
        cipSend(buf);
        if(esp8266_waitFor("SEND OK"))
            do_next = 12;
        else
            processTerminate();        
        break;
    case 12:
        cipSend(".");
        if(esp8266_waitFor("SEND OK"))
            do_next = 13;
        else
            processTerminate();        
        break;
    case 13:
        cipSend("QUIT");
        if(esp8266_waitFor("221")){
            Serial_print("Email sent-DONE!");  
            do_next = 14;
        }
        else
            processTerminate();        
        break;
    default:
        break;
}
}

