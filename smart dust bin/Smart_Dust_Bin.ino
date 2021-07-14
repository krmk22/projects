#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Servo.h>
#include <EEPROM.h>

#include "clock.h"
#include "header.h"
#include "i2c.h"
#include "lcd.h"
#include "mfrc522.h"
#include "keypad.h"
#include "uart.h"
#include "sim800l.h"

#ifndef SecurityConfig
#define SecurityConfig
#define WiFiSSID    "ESP8266"
#define WiFiPass    "PIC16f877a"
#define MailID      ""
#define PassWord    ""
#define Token       ""
#endif

#ifndef VariableConfig
#define VariableConfig
#define DeviceLabel     "dlab"
#define VariableLabel1  "binlevel"
#define VariableLabel2  "status"
#endif

#ifndef LCDConfig
#define LCDConfig
#define LCDData  13
#define LCDClock 12
#endif

#ifndef RFIDConfig
#define RFIDConfig
#define SS      11
#define SCK     10
#define MOSI     9
#define MISO     8
#endif

#ifndef RadarConfig
#define RadarConfig
#define Trigger  7
#define Echo     6
#endif

#ifndef ServoConfig
#define ServoConfig
#define ServoMotor 5
#define DoorOpen   180
#define DoorClose  80 
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm    4
#endif

#ifndef KeypadConfig
#define KeypadConfig
#define KeySET   A0
#define KeyMOV   A1
#define KeyENT   A2
#define KeyINC   A3
#define KeyDEC   A4
#endif

#ifndef RFIDDatas
#define RFIDDatas
#define CARD1 "598445B820"
#define CARD2 "396651B8B6"
#define CARD3 "09C051B820"
#define CARD4 "C9F849B8C0"
#define CARD5 "191943B8FB"
#endif

#ifndef UserConfig
#define UserConfig
#define DoorCloseInterval       10000
#define DustCollectInterval     25000
#endif

extern unsigned char TagArray[16];
unsigned long int SensorTimer, DoorCloseTimer;
unsigned int KeyDebounceTime;
unsigned char LocalArray[12], BinState, LastDetectedKey, BinLevel;
char ISMessageSended, ISDoorOpen, ISCardDetected;

Servo MyServo;

unsigned char FetchDistance()
{
  register unsigned long int PulseDuration = 0;
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);
  cli();
  PulseDuration = pulseIn(Echo, HIGH, 25000);
  sei();
  digitalWrite(Trigger, HIGH);
  return (PulseDuration * 0.034 / 2);
}

char GPRS_Send(const char *DevLabel, unsigned char Data)
{
  if(!GSM_TalkBack("AT+CGATT?\r\n","+CGATT:1", 2500))
  {
    GSM_TalkBack("AT+CGATT=1\r\n","OK",5000);
    Serial_Write("AT+CIPSTART=\"TCP\",");
    Serial_Write("\"industrial.api.ubidots.com\",");
    Serial_Write("9012\r\n");
    GSM_TalkBack("","CONNECT",5000);
  }
  
  register unsigned char LocalArray[5];
  itoa(Data, (char*)LocalArray, DEC);
  unsigned char VariableLength = strlen(Token) + strlen(DeviceLabel) + strlen(DevLabel);
  unsigned char FinalLength = 27 + VariableLength + strlen(LocalArray) + 2;

  Serial_Write("AT+CIPSEND=");
  Serial_Decimal(FinalLength, 3, DEC);
  GSM_TalkBack("\r\n",">",1500);

  Serial_Write("microchip/1.0|POST|");
  Serial_Write(Token); Serial_Send('|');
  Serial_Write(DeviceLabel); Serial_Write("=>");
  Serial_Write(DevLabel); Serial_Send(':');
  Serial_Write(LocalArray); Serial_Send('|');
  Serial_Write("end");

  return GSM_TalkBack("\r\n", "SEND", 2500);
}

char VerifyRFID()
{
  if(strstr((char*)TagArray, CARD1) != '\0') return 1;
  else if(strstr((char*)TagArray, CARD2) != '\0') return 2;
  else if(strstr((char*)TagArray, CARD3) != '\0') return 3;
  else if(strstr((char*)TagArray, CARD4) != '\0') return 4;
  else if(strstr((char*)TagArray, CARD5) != '\0') return 5;
  else return 0;
}

void setup()
{
  pinMode(Alarm, OUTPUT);
  digitalWrite(Alarm, LOW);
  pinMode(ServoMotor, OUTPUT);
  pinMode(Trigger, OUTPUT);
  digitalWrite(Trigger, HIGH);
  pinMode(Echo, INPUT);

  MyServo.attach(ServoMotor);
  MyServo.write(DoorClose);
  ISDoorOpen = False;

  RFID_Initialize(SS, SCK, MOSI, MISO);
  Keypad_Initialize(KeySET, KeyMOV, KeyENT, KeyINC, KeyDEC);
  LCD_Initialize(LCDClock, LCDData);

  LCD_Disp(0x80," SMART DUST BIN  ");
  LCD_Disp(0xC0," AN IOT APPROACH ");
  DelayMS(2500); LCDClear();

  for(unsigned char k = 0; k < 10; k++)
  {
    LCD_Disp(0x80, "+91"); LCD_Disp(0xC0, "+91");
    LCD_Write(0x83 + k, EEPROM.read(0x00 + k));
    LCD_Write(0xC3 + k, EEPROM.read(0x10 + k));
  }
  DelayMS(1000); LCDClear();
  
  if(GSM_Initialize()) LCD_Disp(0x80,"GSM FOUND...");
  else LCD_Disp(0x80,"GSM NOT FOUND"); DelayMS(500); LCDClear();

  if(GSM_CheckSignal()) LCD_Disp(0x80,"SIM TOWERED");
  else LCD_Disp(0x80,"N/W ERROR"); DelayMS(500); LCDClear();

  GSM_TalkBack("AT+CGATT=1\r\n","OK",5000);
  Serial_Write("AT+CIPSTART=\"TCP\",");
  Serial_Write("\"industrial.api.ubidots.com\",");
  Serial_Write("9012\r\n");
  if(GSM_TalkBack("","CONNECT",5000)) LCD_Disp(0x80,"SERVER CONNECTED");
  else LCD_Disp(0x80,"SERVER ERROR"); DelayMS(500); LCDClear();

  BinLevel = FetchDistance();
}

void loop()
{
  LastDetectedKey = FetchKey(&KeyDebounceTime);
  ISCardDetected = ReadRFID();
  BinLevel = FetchDistance();

  if(BinLevel > 9) BinState = 2;
  else if(BinLevel <= 9 && BinLevel > 4) BinState = 1;
  else BinState = 0;

  LCD_Disp(0x80, "BIN_LEVEL:");
  LCD_Decimal(0x8A, BinLevel, 3, DEC);
  LCD_Disp(0x8E, "CM");

  LCD_Disp(0xC0, "BIN_STATE:");
  LCD_Disp(0xCA, (!BinState ?"HIGH" :"NORM"));

  if(ISDoorOpen) LCD_Decimal(0xCE, (DoorCloseTimer - millis()) / 1000, 2, DEC);
  else LCD_Disp(0xCE, "  ");
  
  if(LastDetectedKey == SET && KeyDebounceTime < 300)
  {
    LCDClear();
    for(unsigned char k = 0; k < 10; k++)
    {
     LCD_Disp(0x80, "+91"); LCD_Disp(0xC0, "+91");
     LCD_Write(0x83 + k, EEPROM.read(0x00 + k));
     LCD_Write(0xC3 + k, EEPROM.read(0x10 + k));
    }
    DelayMS(1500); LCDClear();
  }
  if(LastDetectedKey == SET && KeyDebounceTime > 300)
  {
    LCDClear();
    memset(LocalArray, '\0', sizeof(LocalArray));
    LCD_Disp(0x80,"ENTER MOB NUM1 ");
    Keypad(0xC0, LocalArray, 10);
    for(unsigned char k = 0; k < 10; k++)
    EEPROM.write(0x00 + k, LocalArray[k]);
    DelayMS(500); LCDClear();

    memset(LocalArray, '\0', sizeof(LocalArray));
    LCD_Disp(0x80,"ENTER MOB NUM2 ");
    Keypad(0xC0, LocalArray, 10);
    for(unsigned char k = 0; k < 10; k++)
    EEPROM.write(0x10 + k, LocalArray[k]);
    DelayMS(500); LCDClear();
  }


  if(BinState != 2 && !ISMessageSended)
  {
    LCD_Write(0xCF, 'M');

    memset(LocalArray, '\0', sizeof(LocalArray));
    for(unsigned char k = 0; k < 10; k++)
    LocalArray[k] = EEPROM.read(0x00 + k);
    GSM_SendMessage(LocalArray, "AREA-44 DUST BIN REACH MAXIMUM LEVEL...");

    memset(LocalArray, '\0', sizeof(LocalArray));
    for(unsigned char k = 0; k < 10; k++)
    LocalArray[k] = EEPROM.read(0x10 + k);
    GSM_SendMessage(LocalArray, "AREA-44 DUST BIN REACH MAXIMUM LEVEL...");

    LCD_Write(0xCF, ' ');
    ISMessageSended = True;
  }
  else if(BinState == 2 && ISMessageSended) ISMessageSended = False;

  if(ISCardDetected)
  {
    unsigned char LocalVariable = VerifyRFID();

    if(!LocalVariable)
    {
      MyServo.write(DoorClose);
      ISDoorOpen = False;
      digitalWrite(Alarm, HIGH);
      delay(500); delay(500);
      digitalWrite(Alarm, LOW);
    }
    else
    {
      unsigned char ReferenceVariable = MyServo.read();
      MyServo.write(ReferenceVariable == DoorClose ?DoorOpen :DoorClose);
      ISDoorOpen = (MyServo.read() == DoorOpen ?True :False);

      digitalWrite(Alarm, HIGH);
      delay(100); delay(100);
      digitalWrite(Alarm, LOW);

      if(ReferenceVariable == DoorOpen)
      {
        LCD_Write(0xCF, 'W');
        GPRS_Send(VariableLabel1, BinLevel);
        GPRS_Send(VariableLabel2, BinState);
        LCD_Write(0xCF, ' ');
      }
      
      DoorCloseTimer = millis() + (LocalVariable > 3 ?DustCollectInterval :DoorCloseInterval);
    }
  }
  if(ISDoorOpen && millis() > DoorCloseTimer)
  {
     MyServo.write(DoorClose);
     ISDoorOpen = False;
     LCD_Write(0xCF, 'W');
     GPRS_Send(VariableLabel1, BinLevel);
     GPRS_Send(VariableLabel2, BinState);
     LCD_Write(0xCF, ' ');
   }
}

