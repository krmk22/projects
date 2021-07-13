#include <Arduino.h>

#include "i2c.h"

char SerialCLK;
char SerialData;

void I2C_Initialize(char SCK, char SDA)
{
  SerialCLK = SCK;
  SerialData = SDA;
  pinMode(SerialCLK, OUTPUT);
  pinMode(SerialData, OUTPUT);
}

void I2C_Start()
{
  digitalWrite(SerialCLK, HIGH);
  digitalWrite(SerialData, HIGH);
  delayMicroseconds(BusFreeTime);
  digitalWrite(SerialData, LOW);
  delayMicroseconds(StartHoldTime);
  digitalWrite(SerialCLK, LOW);
}

void I2C_Stop()
{
  digitalWrite(SerialCLK, HIGH);
  digitalWrite(SerialData, LOW);
  delayMicroseconds(BusFreeTime);
  digitalWrite(SerialData, HIGH);
  delayMicroseconds(StopHoldTime);
}

void I2C_Restart()
{
  digitalWrite(SerialData, HIGH);
  digitalWrite(SerialCLK, HIGH);
  delayMicroseconds(BusFreeTime);
  digitalWrite(SerialData, LOW);
  delayMicroseconds(RestartHoldTime);
  digitalWrite(SerialCLK, LOW);
}

void I2C_Clock()
{
  delayMicroseconds(DataHoldTime);
  digitalWrite(SerialCLK, HIGH);
  delayMicroseconds(ClockHighTime);
  digitalWrite(SerialCLK, LOW);
  delayMicroseconds(ClockLowTime);
  delayMicroseconds(DataSetupTime);
}

void I2C_Write(unsigned char Data)
{
  for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
  {
    if(Shifting & Data) digitalWrite(SerialData, HIGH);
    else digitalWrite(SerialData, LOW); I2C_Clock();
  }
  digitalWrite(SerialData, LOW);
  I2C_Clock();
}

unsigned char I2C_Read(unsigned char Ack)
{
  unsigned char ReturnVariable = 0x00;
  pinMode(SerialData, INPUT);
  for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
  {
    if(digitalRead(SerialData)) ReturnVariable |= Shifting;
    else ReturnVariable &=~ Shifting; I2C_Clock();
  }
  pinMode(SerialData, OUTPUT);
  if(Ack) digitalWrite(SerialData, LOW);
  else digitalWrite(SerialData, HIGH);
  I2C_Clock();
  return ReturnVariable;
}

void TWI_Write(const unsigned int BusAddress, unsigned char Address, unsigned char Data)
{
  unsigned char ReturnVariable;
  I2C_Start();
  I2C_Write(BusAddress);
  I2C_Write(Address);
  I2C_Write(Data);
  I2C_Stop();
}

unsigned char TWI_Read(const unsigned int BusAddress, unsigned char Address)
{
  I2C_Start();
  I2C_Write(BusAddress);
  I2C_Write(Address);
  I2C_Restart();
  I2C_Write(BusAddress | 1);
  unsigned char ReturnVariable = I2C_Read(1);
  I2C_Stop();
  return ReturnVariable;
}

void TWI_SeqWrite(const unsigned int BusAddress, unsigned char RegAddress, unsigned char *Array, unsigned char Length)
{
  I2C_Start();
  I2C_Write(BusAddress);
  I2C_Write(RegAddress);
  for(unsigned char k = 0; k <= Length; k++)
  I2C_Write(Array[k]);
  I2C_Stop();
}

void TWI_SeqRead(const unsigned int BusAddress, unsigned char RegAddress, unsigned char *Array, unsigned char Length)
{
  register unsigned char LocalVariable;
  
  I2C_Start();
  I2C_Write(BusAddress);
  I2C_Write(RegAddress);
  I2C_Restart();
  I2C_Write(RegAddress | 1);
  for(LocalVariable = 0; LocalVariable <= (Length - 1); LocalVariable++)
  Array[LocalVariable] = I2C_Read(1);
  Array[LocalVariable + 1] = I2C_Read(0);
  I2C_Stop();
} 
  
