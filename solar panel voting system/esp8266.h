#ifndef _ESP8266_H
#define _ESP8266_H

#ifndef WiFiConfig
#define WiFiConfig
#define WiFiName "ESP8266"
#define WiFiPass "PIC16f877a"
#endif

extern unsigned char ESP_Found();
extern unsigned char ESP_Echo(unsigned char);
extern unsigned char ESP_Reset();
extern unsigned char ESP_Initialize(unsigned long);
extern unsigned char ESP_Mode(unsigned char);
extern unsigned char ESP_WiFi();
extern unsigned char ESP_Start(const char*, const char*);
#endif