#include <Arduino.h>
#include <string.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include "esp.h"

unsigned char ESP_Found()
{
  Serial_Write("AT");
  Serial_Flush();
  return STalkBack(True, "OK", 2, 2500);
}

unsigned char ESP_Echo(unsigned char Status)
{
  Serial_Write("ATE");
  Serial_Send(Status ?'1' :'0');
  Serial_Flush();
  return STalkBack(True, "OK", 2, 2500);
}

unsigned char ESP_Reset()
{
  Serial_Write("AT+RST");
  Serial_Flush();
  return STalkBack(True, "ready", 5, 2500);
}

unsigned char ESP_Initialize(unsigned long ReqBaud)
{
  Serial_Initialize(ReqBaud);
  Serial_Enable(True);
  ESP_Echo(False);
  Serial_Write("AT+CIPMUX=0");
  Serial_Flush();
  return STalkBack(True, "OK", 2, 2500);
}

unsigned char ESP_Mode(unsigned char DesireMode)
{
  if(!DesireMode || DesireMode > 3) DesireMode = 3;
  Serial_Write("AT+CWMODE=");
  Serial_Send(DesireMode + '0'); 
  Serial_Flush();
  return STalkBack(True, "OK", 2, 2500);
}

unsigned char ESP_WiFi()
{
  Serial_Write("AT+CWJAP?"); Serial_Flush();
  if(STalkBack(True, WiFiName, strlen(WiFiName), 5000)) return True;
  
  Serial_Write("AT+CWLAP=\"");
  Serial_Write(WiFiName);
  Serial_Write("\"");
  Serial_Flush();
  if(!STalkBack(True, WiFiName, strlen(WiFiName), 5000)) return False;
  
  Serial_Write("AT+CWJAP=\"");
  Serial_Write(WiFiName);
  Serial_Write("\",\"");
  Serial_Write(WiFiPass);
  Serial_Write("\"");
  Serial_Flush();
  return STalkBack(True, "OK", 2, 25000);
}

unsigned char ESP_Start(const char *Address, const char *Port)
{
  Serial_Write("AT+CIPSTART=");
  Serial_Write("\"TCP\",\"");
  Serial_Write(Address);
  Serial_Write("\",");
  Serial_Write(Port);
  Serial_Flush();
  return STalkBack(True, "CONNECT", 7, 10000);
} 
