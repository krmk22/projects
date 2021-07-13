/*************************************/
/********PETROL BULK AUTOMATION*******/
/*************************************/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "header.h"
#include "lcd.h"
#include "matrix.h"
#include "uart.h"

#ifndef CardDetails
#define CardDetails
#define Card1 "010070728D8E"
#define Card2 "0100707F141A"
#define Card3 "0100705A6B40"
#define Card4 "010070978E68"
#define Card5 "010070697860"
#endif

#ifndef SecureKeyConfig
#define SecureKeyConfig
#define SecureCard1 1553
#define SecureCard2 1744
#define SecureCard3 2344
#define SecureCard4 3486
#define SecureCard5 7698
#define SuperSecure 2205
#endif

#ifndef LCDConfig
#define LCDConfig
#define RS A0
#define EN A1
#define D4 A2
#define D5 A3
#define D6 A4
#define D7 A5
#endif

#ifndef MatrixConfig
#define MatrixConfig
#define MatrixRow1 9
#define MatrixRow2 8
#define MatrixRow3 7
#define MatrixRow4 6
#define MatrixCol1 13
#define MatrixCol2 12
#define MatrixCol3 11
#define MatrixCol4 10
#endif

#ifndef OutputConfig
#define OutputConfig
#define FuelPump 5
#define Alarm 4
#endif

unsigned char SerialArray[20], SerialCount;
unsigned int DetectedKey;

unsigned int FetchKeypad();
char ProcessFuelPumping();

ISR(USART_RX_vect)
{
  SerialArray[SerialCount] = Serial_Receive();
  SerialCount = (SerialCount < 20 ?SerialCount + 1 :0);
}

void setup()
{
  pinMode(FuelPump, OUTPUT);
  digitalWrite(FuelPump, LOW);

  pinMode(Alarm, OUTPUT);
  digitalWrite(Alarm, LOW);

  Serial_Initialize();
  Serial_Enable(True);
  
  Matrix_Initialize();
  LCD_Initialize();
  LCD_Disp(0x80,"RFID BASE PETROL");
  LCD_Disp(0xC0,"BULK A/M SYSTEM ");
  delay(3000); LCDClear();
}

void loop()
{
  if(strstr((char*)SerialArray, Card1) != '\0')
  {
    LCD_Disp(0x80," ENTER U R PIN  ");
    DetectedKey = FetchKeypad();
    
    if(DetectedKey == SecureCard1 || DetectedKey == SuperSecure)
    {
      LCD_Disp(0x80,"PIN MATCHED  "); delay(500);
      if(ProcessFuelPumping())
      LCD_Disp(0x80,"PROCESS COMPLETE");
      digitalWrite(Alarm, HIGH);
    }
    else
    {
      LCD_Disp(0x80,"PIN NOT MATCHED ");
      digitalWrite(Alarm, HIGH); delay(500);
    }
    
    delay(500); digitalWrite(Alarm, LOW);
    memset(SerialArray, '\0', sizeof(SerialArray));
    SerialCount = 0; LCDClear();
  }
  if(strstr((char*)SerialArray, Card2) != '\0')
  {
    LCD_Disp(0x80," ENTER U R PIN  ");
    DetectedKey = FetchKeypad();
    
    if(DetectedKey == SecureCard2 || DetectedKey == SuperSecure)
    {
      LCD_Disp(0x80,"PIN MATCHED     "); delay(500);
      if(ProcessFuelPumping())
      LCD_Disp(0x80,"PROCESS COMPLETE");
      digitalWrite(Alarm, HIGH);
    }
    else
    {
      LCD_Disp(0x80,"PIN NOT MATCHED ");
      digitalWrite(Alarm, HIGH); delay(500);
    }
    
    delay(500); digitalWrite(Alarm, LOW);
    memset(SerialArray, '\0', sizeof(SerialArray));
    SerialCount = 0; LCDClear();
  }
  if(strstr((char*)SerialArray, Card3) != '\0')
  {
    LCD_Disp(0x80," ENTER U R PIN  ");
    DetectedKey = FetchKeypad();
    
    if(DetectedKey == SecureCard3 || DetectedKey == SuperSecure)
    {
      LCD_Disp(0x80,"PIN MATCHED     "); delay(500);
      if(ProcessFuelPumping())
      LCD_Disp(0x80,"PROCESS COMPLETE");
      digitalWrite(Alarm, HIGH);
    }
    else
    {
      LCD_Disp(0x80,"PIN NOT MATCHED ");
      digitalWrite(Alarm, HIGH); delay(500);
    }
    
    delay(500); digitalWrite(Alarm, LOW);
    memset(SerialArray, '\0', sizeof(SerialArray));
    SerialCount = 0; LCDClear();
  }
  if(strstr((char*)SerialArray, Card4) != '\0')
  {
    LCD_Disp(0x80," ENTER U R PIN  ");
    DetectedKey = FetchKeypad();
    
    if(DetectedKey == SecureCard4 || DetectedKey == SuperSecure)
    {
      LCD_Disp(0x80,"PIN MATCHED     "); delay(500);
      if(ProcessFuelPumping())
      LCD_Disp(0x80,"PROCESS COMPLETE");
      digitalWrite(Alarm, HIGH);
    }
    else
    {
      LCD_Disp(0x80,"PIN NOT MATCHED ");
      digitalWrite(Alarm, HIGH); delay(500);
    }
    
    delay(500); digitalWrite(Alarm, LOW);
    memset(SerialArray, '\0', sizeof(SerialArray));
    SerialCount = 0; LCDClear();
  }
  if(strstr((char*)SerialArray, Card5) != '\0')
  {
    LCD_Disp(0x80," ENTER U R PIN  ");
    DetectedKey = FetchKeypad();
    
    if(DetectedKey == SecureCard5 || DetectedKey == SuperSecure)
    {
      LCD_Disp(0x80,"PIN MATCHED     "); delay(500);
      if(ProcessFuelPumping())
      LCD_Disp(0x80,"PROCESS COMPLETE");
      digitalWrite(Alarm, HIGH);
    }
    else
    {
      LCD_Disp(0x80,"PIN NOT MATCHED ");
      digitalWrite(Alarm, HIGH); delay(500);
    }
    
    delay(500); digitalWrite(Alarm, LOW);
    memset(SerialArray, '\0', sizeof(SerialArray));
    SerialCount = 0; LCDClear();
  }
}

unsigned int FetchKeypad()
{
  unsigned int ReturnVariable;
  unsigned char RequireArray[6];
  unsigned char Variable;
  unsigned int KeyFetchTime;
  unsigned char LocalCount;
  
  do
  {
    Variable = Matrix_FetchKey(&KeyFetchTime);

    if(isdigit(Variable))
    {
      RequireArray[LocalCount] = Variable;
      LCD_Write(0xC0 + LocalCount, Variable);
      LocalCount++; delay(200);
    }
    if(Variable == '*')
    {
      if(--LocalCount < 0) LocalCount = 0;
      RequireArray[LocalCount] = 0;
      LCD_Write(0xC0 + LocalCount, ' ');
    }
    if(Variable == '#')
    {
      LCD_Disp(0xC0,"                ");
      memset(RequireArray, '\0', sizeof(RequireArray));
      LocalCount = 0;
    }
  }
  while(LocalCount != 4); delay(200);
  ReturnVariable = atoi((char*)RequireArray);
  LCD_Command(0x0C);
  return ReturnVariable;
}

char ProcessFuelPumping()
{
  unsigned int RequireAmount;
  float Fuel4EnteredPrice; 
  
  LCDClear(); 
  LCD_Disp(0x80,"PRICE: 50RS P/L ");
  RequireAmount = FetchKeypad();
  Fuel4EnteredPrice = RequireAmount / 50.00F;
  LCDClear(); 
  
  digitalWrite(Alarm, HIGH);
  delay(500);
  digitalWrite(Alarm, LOW);
  delay(500);
  digitalWrite(FuelPump, HIGH);

  do
  {
    Fuel4EnteredPrice -= 0.02F;
    RequireAmount -= 1;
    
    LCD_Decimal(0xC0, RequireAmount, 4, DEC);
    LCD_Float(0xC5, Fuel4EnteredPrice, 2, 2, NoSign);
    
    delay(100);
  }
  while(RequireAmount);
  digitalWrite(FuelPump, LOW);
  return True;
}

