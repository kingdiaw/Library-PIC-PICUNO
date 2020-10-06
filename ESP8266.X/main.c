#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "ESP8266_functions.h"

#define _XTAL_FREQ          64000000
#define _mcu_baudRate       115200
#define _esp8266_baudRate   115200
#define PB1                 PORTDbits.RD1
#define PB1_TRIS            TRISDbits.TRISD1
#define LED                 LATDbits.LATD0
#define LED_TRIS            TRISDbits.TRISD0

const char* ssid="Kdw";
const char* password="aa112233";
//const char* ip="smtp.elasticemail.com";
//const char* port="2525";

//***Initialize INTERNAL OSC***//
void INTOSC_init(void)
{
    // SCS FOSC; IRCF 16MHz_HFINTOSC; IDLEN disabled; 
    OSCCON = 0x70;
    // PRISD enabled; SOSCGO disabled; MFIOSEL disabled; 
    OSCCON2 = 0x04;
    // INTSRC disabled; PLLEN enabled; TUN 0; 
    OSCTUNE = 0x40;
    // Wait for PLL to stabilize
    while(PLLRDY == 0);
}
//***Initializing UART Module for PIC18F45K22***//
void UART_init(void)
{
    //***Setting IO pins for UART***//
    TRISCbits.TRISC6 = 0;   //TX pin as output
    TRISCbits.TRISC7 = 1;   //RX pin as input
    
    //***Initialize SPBGH1:SPBRG1 register ***//
    SPBRGH1 = ((_XTAL_FREQ/4)/_mcu_baudRate-1)/256; //6
    SPBRG1 = ((_XTAL_FREQ/4)/_mcu_baudRate-1)%256;   //130
    BAUDCON1bits.BRG16 = 1; //16-bit BRG
    TXSTA1bits.BRGH = 1;    //High Speed

    //***Enable Asynchronous serial Port***//
    TXSTA1bits.SYNC = 0;    //Asynchronous
    RCSTA1bits.SPEN = 1;    //Enable serial port pins
    
    //***Enable Transmission and Reception***//
    TXSTA1bits.TXEN = 1;    //Enable Transmission
    RCSTA1bits.CREN = 1;    //Enable Reception
    
    //***Select 8-bit mode***//
    TXSTA1bits.TX9 = 0;     //8-bit transmission selected
    RCSTA1bits.RX9 = 0;     //8-bit reception selected
}

//***Function to send one byte of data to UART***//
void UART_send_char(char data)
{
    while(PIR1bits.TX1IF == 0);
    TXREG1 = data;
}

//***Function to get one byte of data from UART***//
char UART_get_char()
{   
    while(!PIR1bits.RC1IF);
    
    if(RCSTA1bits.OERR)
    {
        RCSTA1bits.CREN = 0;
        RCSTA1bits.CREN = 1;
    }
    return RCREG1;  //receive data and send it to caller function
}
//***Function to check data receive***//
bool UART_ready(void)
{
    return PIR1bits.RC1IF;
}

//***Function to convert string to byte***//
void UART_send_string(char *pointer)
{
    while(*pointer)//if there is a char
        UART_send_char(*pointer++);
}

void esp8266_feedback(unsigned char check)
{
    switch(check)
    {
        case 1:UART_send_string("OK\r\n");
        break;
        case 2:UART_send_string("ready\r\n");
        break;
        case 3:UART_send_string("FAIL\r\n");
        break;
        case 4:UART_send_string("no change\r\n");
        break;  
        case 5:UART_send_string("Linked\r\n");
        break; 
        case 6:UART_send_string("Unlinked\r\n");
        break;        
    }
}

////***Initializing UART2 Module for PIC18F45K22***//
//void _esp8266_init(void)
//{
//    //***Setting IO pins for UART***//
//    TRISDbits.TRISD6 = 0;   //TX pin as output
//    TRISDbits.TRISD7 = 1;   //RX pin as input
//    
//    //***Initialize SPBGH1:SPBRG1 register ***//
//    SPBRGH2 = ((_XTAL_FREQ/4)/_mcu_baudRate-1)/256; //6
//    SPBRG2 = ((_XTAL_FREQ/4)/_mcu_baudRate-1)%256;   //130
//    BAUDCON2bits.BRG16 = 1; //16-bit BRG
//    TXSTA2bits.BRGH = 1;    //High Speed
//
//    //***Enable Asynchronous serial Port***//
//    TXSTA2bits.SYNC = 0;    //Asynchronous
//    RCSTA2bits.SPEN = 1;    //Enable serial port pins
//    
//    //***Enable Transmission and Reception***//
//    TXSTA2bits.TXEN = 1;    //Enable Transmission
//    RCSTA2bits.CREN = 1;    //Enable Reception
//    
//    //***Select 8-bit mode***//
//    TXSTA2bits.TX9 = 0;     //8-bit transmission selected
//    RCSTA2bits.RX9 = 0;     //8-bit reception selected
//}
//
////***Function to send one byte of data to ESP8266***//
//void _esp8266_send_char(char data)
//{
//    while(PIR3bits.TX2IF == 0);
//    TXREG2 = data;    
//}
//
////***Function to convert string to byte***//
//void _esp8266_send_string(char *pointer)
//{
//    while(*pointer)//if there is a char
//        _esp8266_send_char(*pointer++);
//}
////***Function to get one byte of data from ESP8266***//
//char _esp8266_get_char()
//{   
//    while(!PIR3bits.RC2IF);
//    
//    if(RCSTA2bits.OERR)
//    {
//        RCSTA2bits.CREN = 0;
//        RCSTA2bits.CREN = 1;
//    }
//    return RCREG2;  //receive data and send it to caller function
//}
////***Function to check data receive***//
//bool _esp8266_ready(void)
//{
//    return PIR3bits.RC2IF;
//}

//***Start MAIN function***//
unsigned char buf[]=0;
void main(void) 
{
    char incomingByte;
    INTOSC_init();
    TRISBbits.TRISB7 = 0;
    PB1_TRIS = 1;
    LED_TRIS = 0;
    LED=0;
    UART_init();
    Initialize_ESP8266();
    UART_send_string("Setup Done\r\n");
    UART_send_string("Press PB1 to Start\r\n");
    while(PB1);
    buf[0]=0;
    strcat(buf,"AT\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());
    __delay_ms(1500);
    buf[0]=0;
    strcat(buf,"AT+CWMODE=3\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());
    __delay_ms(1500);
    buf[0]=0;
    strcat(buf,"AT+CWJAP=\"");
    strcat(buf,ssid);
    strcat(buf,"\",\"");
    strcat(buf,password);
    strcat(buf,"\"\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());  
    __delay_ms(1500);
    buf[0]=0;
    strcat(buf,"AT+CIPMUX=1\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());
    __delay_ms(1500);
    
    buf[0]=0;
    strcat(buf,"AT+CIPSERVER=1,80\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());
    __delay_ms(1500);
    
    buf[0]=0;
    strcat(buf,"AT+CIPSTART=4,\"TCP\",\"");
    strcat(buf,"smtp.elasticemail.com");
    strcat(buf,"\",");
    strcat(buf,"2525");
    strcat(buf,"\r\n");
    _esp8266_print(buf);
    UART_send_string(buf);
    esp8266_feedback(_esp8266_waitResponse());
    __delay_ms(1500);
//    char cipStart[60]="AT+CIPSTART=4,\"TCP\",\"";
//    strcat(cipStart,ip);
//    strcat(cipStart,"\",");
//    strcat(cipStart,port);
//    strcat(cipStart,"\r\n");
//    _esp8266_print(cipStart);
//    UART_send_string(cipStart);
//    esp8266_feedback(_esp8266_waitResponse());     
//    __delay_ms(1000);    
    
    LED=1;
    while(1);
//    _esp8266_send_string("AT\r\n");__delay_ms(1500);LATDbits.LD0^=1; 
//    _esp8266_send_string("AT+CIPSERVER=1,80\r\n");__delay_ms(1500);LATDbits.LD0^=1; 
//    _esp8266_send_string("AT+CIPSTART=4,\"TCP\",\"smtp.gmail.com\",587\r\n");__delay_ms(1500);LATDbits.LD0^=1; 
//    __delay_ms(1500);
//    _esp8266_send_string("AT+CIPSEND=4,20\r\n");__delay_ms(1000);LATDbits.LD0^=1; 
//    _esp8266_send_string("EHLO 74.125.24.108\r\n");__delay_ms(1500);LATDbits.LD0^=1; 
//    _esp8266_send_string("AT+CIPSEND=4,10\r\n");__delay_ms(1000);LATDbits.LD0^=1; 
//    _esp8266_send_string("STARTTLS\r\n");
    while(1)
    {
//        if(UART_ready())
//        {
//            _esp8266_send_char(UART_get_char());
//        }
//        if(_esp8266_ready())
//        {
//            UART_send_char(_esp8266_get_char());
//        }
         /*Check if the ESP_PIC communication is successful*/

    }
}

static void interrupt ISR(void)
{
    
}
