#include <Arduino.h>
#include <avr/io.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#include "spi.h"

char SerialClock;
char DataOutput;
char DataInput;

void SPI_Initialize(char SCK, char MOSI, char MISO)
{
  SerialClock = SCK;
  DataOutput = MOSI;
  DataInput = MISO;
  
  pinMode(SerialClock, OUTPUT);
  pinMode(DataOutput, OUTPUT);
  pinMode(DataInput, INPUT);

  digitalWrite(DataOutput, HIGH);
  digitalWrite(SerialClock, LOW);
}

void SPI_Write(unsigned char WriteData)
{
  for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
  {
    if(Shifting & WriteData) digitalWrite(DataOutput, HIGH);
    if(!(Shifting & WriteData)) digitalWrite(DataOutput, LOW);
    digitalWrite(SerialClock, HIGH); delayMicroseconds(5);
    digitalWrite(SerialClock, LOW); delayMicroseconds(5);
  }
}

unsigned char SPI_Read()
{
  unsigned char ReturnVariable = 0x00;
  for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
  {
    if(digitalRead(DataInput)) ReturnVariable |= Shifting;
    if(!digitalRead(DataInput)) ReturnVariable &=~ Shifting;
    digitalWrite(SerialClock, HIGH); delayMicroseconds(5);
    digitalWrite(SerialClock, LOW); delayMicroseconds(5);
  }
  return ReturnVariable;
}

unsigned char SPI_RW(unsigned char WriteData)
{
  unsigned char ReturnVariable = 0x00;
  for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
  {
    if(Shifting & WriteData) digitalWrite(DataOutput, HIGH);
    if(!(Shifting & WriteData)) digitalWrite(DataOutput, LOW);
    digitalWrite(SerialClock, HIGH); delayMicroseconds(5);
    if(digitalRead(DataInput)) ReturnVariable |= Shifting;
    if(!digitalRead(DataInput)) ReturnVariable &=~ Shifting;
    digitalWrite(SerialClock, LOW); delayMicroseconds(5);
  }
  return ReturnVariable;
}

