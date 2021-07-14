#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include "mqtt.h"

unsigned char MQTT_Connect()
{
  const char *LocalArray = "\x00\x04\x4D\x51\x54\x54\x04\x82";
  unsigned char Length = 2 + 8 + 2 + 2 + strlen(ClientName) + 2 + strlen(ClientID);
  
  Serial_Write("AT+CIPSEND=");
  Serial_Decimal(Length, 3, DEC);
  STalkBack(True, ">", 1, 2500);
  Serial_Flush();
  
  Serial_Send(0x10); Serial_Send(Length - 2);
  Serial_Writeln(LocalArray, 8);
  Serial_Send(0x00); Serial_Send(0x00);
  
  Serial_Send(strlen(ClientName) >> 8);
  Serial_Send(strlen(ClientName) & 0xFF);
  Serial_Writeln(ClientName, strlen(ClientName));
  
  Serial_Send(strlen(ClientID) >> 8);
  Serial_Send(strlen(ClientID) & 0xFF);
  Serial_Writeln(ClientID, strlen(ClientID));
  
  return STalkBack(False, "\x20\x02\x00\x00", 4, 2500);
}

unsigned char MQTT_Publish(const char *VarLab, unsigned int PayLoad)
{
  unsigned char LocalArray[10];
  itoa(PayLoad, (char*)LocalArray, DEC);
  
  unsigned int TopicLength   = strlen(Topic) + strlen(DeviceLabel);
  unsigned int PayLoadLength = strlen(VarLab) + strlen(LocalArray) + 15;
  unsigned int Length        = 2 + 2 + TopicLength + 2 + PayLoadLength;
  
  Serial_Write("AT+CIPSEND=");
  Serial_Decimal(Length, 3, DEC);
  STalkBack(True, ">", 1, 2500); 
  Serial_Flush();
  
  Serial_Send(0x32); Serial_Send(Length - 2);
  Serial_Send(TopicLength >> 8);
  Serial_Send(TopicLength & 0xFF);
  Serial_Writeln(Topic, strlen(Topic));
  Serial_Writeln(DeviceLabel, strlen(DeviceLabel));
  Serial_Send(0x00); Serial_Send(0x01);
  
  Serial_Write("{\"");
  Serial_Writeln(VarLab, strlen(VarLab));
  Serial_Write("\":{\"value\":");
  Serial_Writeln((char*)LocalArray, strlen(LocalArray));
  Serial_Write("}}");
  
  return STalkBack(False, "\x40\x02\x00\x01", 4, 2500);
}

unsigned char MQTT_Subscribe(const char *VarLab, unsigned int MessageID)
{
  unsigned char LocalArray[10];
  memset(LocalArray, '\0', sizeof(LocalArray));
  
  unsigned int TopicLength = strlen(Topic) + strlen(DeviceLabel) + strlen(VarLab) + 4;
  unsigned int Length      = 2 + 2 + 2 + TopicLength + 1;
  
  Serial_Write("AT+CIPSEND=");
  Serial_Decimal(Length, 3, DEC);
  STalkBack(True, ">", 1, 2500);
  Serial_Flush();
  
  Serial_Send(0x82); Serial_Send(Length - 2);
  Serial_Send(MessageID >> 8);
  Serial_Send(MessageID & 0xFF);
  Serial_Send(TopicLength >> 8);
  Serial_Send(TopicLength & 0xFF);
  
  Serial_Writeln(Topic, strlen(Topic));
  Serial_Writeln(DeviceLabel, strlen(DeviceLabel));
  Serial_Send('/'); Serial_Writeln(VarLab, strlen(VarLab));
  Serial_Write("/lv"); Serial_Send(0x00);
  
  memcpy(LocalArray, "\x90\x03\x00\x00\x00", 5);
  LocalArray[2] = MessageID >> 8;
  LocalArray[3] = MessageID & 0xFF;
  
  return STalkBack(False, (char*)LocalArray, 5, 2500);
}   

unsigned char MQTT_Receive(const char *DevLab, unsigned int *ReturnVariable)
{
  unsigned char LocalArray[6];
  
  if(smemmem(SerialArray, SerialCount, "/lv", 3) != '\0')
  {
    DelayMS(25); DelayMS(25); DelayMS(25); DelayMS(25);
    if(smemmem(SerialArray, SerialCount, DevLab, strlen(DevLab)) != '\0')
    {
      unsigned char *RefPointer = smemrchr((char*)SerialArray, '/', SerialCount);
      RefPointer += 3; memset(LocalArray, '\0', sizeof(LocalArray));
      
      for(unsigned char LocalVariable = 0; *RefPointer != '.'; LocalVariable++)
      LocalArray[LocalVariable] = *RefPointer++;
      
      Serial_Flush();
      *ReturnVariable = atoi((char*)LocalArray);
      return True;
    }
  }
  *ReturnVariable = 0;
  return False;
}     
    
unsigned char MQTT_Location(const char *VarLab, float Lattitude, float Longitude)
{
  unsigned char Lat[8];
  unsigned char Lon[8];
  
  ftostra(Lattitude, 2, 4, Lat, NoSign);
  ftostra(Lattitude, 2, 4, Lon, NoSign);
  
  unsigned int TopicLength     = strlen(Topic) + strlen(DeviceLabel);
  unsigned int PayLoadLength   = strlen(VarLab) + strlen(Lat) + strlen(Lon) + 33;
  unsigned int Length          = 2 + 2 + TopicLength + 2 + PayLoadLength;
  
  Serial_Write("AT+CIPSEND=");
  Serial_Decimal(Length, 3, DEC);
  STalkBack(True, ">", 1, 2500);
  Serial_Flush();
  
  Serial_Send(0x32); Serial_Send(Length);
  Serial_Send(TopicLength >> 8);
  Serial_Send(TopicLength & 0xFF);
  Serial_Writeln(Topic, strlen(Topic));
  Serial_Writeln(DeviceLabel, strlen(DeviceLabel));
  Serial_Send(0x00); Serial_Send(0x01);
  
  Serial_Write("{\"");
  Serial_Writeln(VarLab, strlen(VarLab));
  Serial_Write("\":{\"context\":");
  Serial_Write("{\"lat:\":");
  Serial_Writeln(Lat, strlen(Lat));
  Serial_Write(",\"lng:\"");
  Serial_Writeln(Lon, strlen(Lon));
  Serial_Write("}}}");
  
  return STalkBack(False, "\x40\x02\x00\x01", 4, 2500);
} 
