#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _SPI_H
#include "spi.h"
#endif

#include "mfrc522.h"

unsigned char TagArray[16], ResultArray[5];
static unsigned long int LastTimer, TimerDuration;
char SlaveSelect;

void RegisterWrite(unsigned char Address, unsigned char Value)
{
  digitalWrite(SlaveSelect, LOW);
  SPI_Write((Address << 1) & 0x7E);
  SPI_Write(Value);
  digitalWrite(SlaveSelect, HIGH); 
}

unsigned char RegisterRead(unsigned char Address)
{
  unsigned char ReturnVariable;
  digitalWrite(SlaveSelect, LOW);
  SPI_Write(((Address << 1) & 0x7E) | 0x80);
  ReturnVariable = SPI_Read();
  digitalWrite(SlaveSelect, HIGH);
  return ReturnVariable;
}

void RegisterMaskS(unsigned char Address, unsigned char Mask)
{
  unsigned char LastValue = RegisterRead(Address);
  RegisterWrite(Address, LastValue | Mask);
}

void RegisterMaskC(unsigned char Address, unsigned char Mask)
{
  unsigned char LastValue = RegisterRead(Address);
  RegisterWrite(Address, LastValue & (~Mask));
}

unsigned char RFID_Initialize(char SS, char SCK, char MOSI, char MISO, unsigned long Timer)
{
  SlaveSelect = SS;
  pinMode(SlaveSelect, OUTPUT);
  digitalWrite(SlaveSelect, HIGH);
  SPI_Initialize(SCK, MOSI, MISO);

  RegisterWrite(0x01, 0x0F); delay(20);    
  RegisterWrite(0x2A, 0x8D); delay(20);
  RegisterWrite(0x2B, 0x3E); delay(20);
  RegisterWrite(0x2C, 0x00); delay(20);
  RegisterWrite(0x2D, 0x30); delay(20);
  RegisterWrite(0x15, 0x40); delay(20);
  RegisterWrite(0x11, 0x3D); delay(20);
  RegisterWrite(0x26, 0x70); delay(20);
  RegisterMaskS(0x14, 0x03); delay(20);

  TimerDuration = Timer;
  LastTimer = millis() + TimerDuration;
  return RegisterRead(0x37);
}

unsigned char CommandTag(unsigned char *Data, size_t DLength, unsigned char *Result, unsigned char *RLength)
{
  unsigned char Status = RFID_ERR;
  unsigned char InterruptRequestEnable = 0x77;
  unsigned char WaitInterruptRequest = 0x30;
  unsigned char LastBits;
  unsigned char Number;
  unsigned char Length;

  RegisterWrite(0x02, InterruptRequestEnable | 0x80);
  RegisterMaskC(0x04, 0x80);
  RegisterMaskS(0x0A, 0x80);
  RegisterWrite(0x01, 0x00);

  for(Length = 0; Length < DLength; Length++)
  RegisterWrite(0x09, Data[Length]);

  RegisterWrite(0x01, 0x0C);
  RegisterMaskS(0x0D, 0x80);

  cli();
  Length = 25;
  do
  {
    delay(1);
    Number = RegisterRead(0x04);
    Length--;
  }
  while((Length != 0) && !(Number & 0x01) && !(Number & WaitInterruptRequest));
  sei();

  RegisterMaskC(0x0D, 0x80);

  if(Length != 0)
  {
    if(!(RegisterRead(0x06) & 0x1D))
    {
      Status = RFID_OK;
      if(Number & InterruptRequestEnable & 0x01) Status = RFID_NOTAG;
      
      Number = RegisterRead(0x0A);
      LastBits = RegisterRead(0x0C) & 0x07;

      if(LastBits) *RLength = (Number - 1) * 8 + LastBits;
      else *RLength = Number * 8;

      if(Number == 0) Number = 1;
      if(Number > 16) Number = 16;

      for(Length = 0; Length < Number; Length++)
      Result[Length] = RegisterRead(0x09);
    }
  }
  return Status;
}

unsigned char RequestTag(unsigned char Mode, unsigned char *Data)
{
  unsigned char Status, Length;
  RegisterWrite(0x0D, 0x07);
  Data[0] = Mode;
  Status = CommandTag(Data, 1, Data, &Length);
  if((Status != RFID_OK) || (Length != 0x10)) Status = RFID_ERR;
  return Status;
}

unsigned char AntiCollisionTag(unsigned char *Data)
{
  unsigned char Status;
  unsigned char Number;
  unsigned char Length;
  unsigned char CheckByte = 0x00;

  RegisterWrite(0x0D, 0x00);
  Data[0] = 0x93;
  Data[1] = 0x20;
  Status = CommandTag(Data, 2, Data, &Length);
  Length = Length / 8;

  if(Status == RFID_OK)
  {
    for(Number = 0; Number < Length - 1; Number++)
    CheckByte ^= Data[Number];
    if(CheckByte != Data[Number]) Status = RFID_ERR;
  }
  return Status;
}

void CalculateCRC(unsigned char *Data, size_t Length, unsigned char *Result)
{
  unsigned char LLength;
  unsigned char Number;

  RegisterMaskC(0x03, 0x04);
  RegisterMaskS(0x0A, 0x80);

  for(LLength = 0; LLength < Length; LLength++)
  RegisterWrite(0x09, Data[LLength]);

  RegisterWrite(0x01, 0x03);
  LLength = 0xFF;
  do
  {
    Number = RegisterRead(0x03);
    LLength--;
  }
  while((LLength != 0) && !(Number & 0x04));

  Result[0] = RegisterRead(0x22);
  Result[1] = RegisterRead(0x21);
}

unsigned char HaltTag()
{
  unsigned char Status;
  unsigned char Length;
  unsigned char Buffer[4];

  Buffer[0] = 0x50;
  Buffer[1] = 0x00;
  CalculateCRC(Buffer, 2, &Buffer[2]);
  Status = CommandTag(Buffer, 4, Buffer, &Length);
  RegisterMaskC(0x08, 0x08);
  return Status;
}

unsigned char ReadRFID()
{
  unsigned char ReturnVariable = RFID_OK;
  
  if(LastTimer < millis())
  {
    if(SlaveSelect == 7) SlaveSelect = 6;
    else SlaveSelect = 7;
    
    memset(TagArray, '\0', sizeof(TagArray));
    memset(ResultArray, '\0', sizeof(ResultArray));
    
    if(RequestTag(0x26, TagArray) == RFID_OK)
    {
      unsigned char Result = AntiCollisionTag(TagArray);
      if(Result != RFID_OK) return RFID_OK;
      memcpy(ResultArray, TagArray, 5);
      HaltTag();
      
      for(char j = 0, k = 0; j < 5, k < 10; j += 1, k += 2)
      {
        TagArray[k] = ((ResultArray[j] / 16) % 16);
        TagArray[k + 1] = ResultArray[j] % 16;

        if(TagArray[k] >= 10) TagArray[k] += 'A' - '0' - 10;
        if(TagArray[k + 1] >= 10) TagArray[k + 1] += 'A' - '0' - 10;

        TagArray[k] += '0';
        TagArray[k + 1] += '0';
      }
      ReturnVariable = SlaveSelect;
    }
    LastTimer = millis() + TimerDuration;
  }
  return ReturnVariable;
}
