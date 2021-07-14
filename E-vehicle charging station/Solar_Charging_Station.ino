#include <Arduino.h>
#include <avr/io.h>
#include <EEPROM.h>

#include "utils.h"
#include "i2c.h"
#include "keypad.h"
#include "lcd.h"
#include "spi.h"
#include "mfrc522.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define LCDData  13
#define LCDClock 12
#endif

#ifndef RFIDConfig
#define RFIDConfig
#define SS   11
#define SCK  10
#define MOSI  9
#define MISO  8
#endif

#ifndef KeypadConfig
#define KeypadConfig
#define KeySET A0
#define KeyMOV A1
#define KeyENT A2
#define KeyINC A3
#define KeyDEC A4
#endif

#ifndef PortConfig
#define PortConfig
#define Port1 7
#define Port2 6
#define Port3 5
#define Port4 4
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm 3
#endif

#ifndef RFIDData
#define RFIDData
#define Card1 "650AAE21E0"
#define Card2 "65F80E21B2"
#define Card3 "65F1AB211E"
#define Card4 "654C8E2186"
#define Card5 "65D0CF215B"
#endif

extern unsigned char TagArray[16];
unsigned int KeyDebounceTime, CardAmount[10];
unsigned char LastDetectedKey, LastDetectedCard;
char ISPort1Available, ISPort2Available;
char ISPort3Available, ISPort4Available;

unsigned long Port1Timer, Port2Timer;
unsigned long Port3Timer, Port4Timer;

unsigned char RFIDRead()
{
  if(strstr((char*)TagArray, Card1) != '\0') return 1;
  else if(strstr((char*)TagArray, Card2) != '\0') return 2;
  else if(strstr((char*)TagArray, Card3) != '\0') return 3;
  else if(strstr((char*)TagArray, Card4) != '\0') return 4;
  else if(strstr((char*)TagArray, Card5) != '\0') return 5;
  return 0;
}

void setup()
{
  pinMode(Alarm, OUTPUT);
  pinMode(Port1, OUTPUT);
  pinMode(Port2, OUTPUT);
  pinMode(Port3, OUTPUT);
  pinMode(Port4, OUTPUT);

  digitalWrite(Alarm, LOW);
  digitalWrite(Port1, LOW);
  digitalWrite(Port2, LOW);
  digitalWrite(Port3, LOW);
  digitalWrite(Port4, LOW);

  LCD_Initialize(LCDClock, LCDData);
  Keypad_Initialize(KeySET, KeyMOV, KeyENT, KeyINC, KeyDEC);
  RFID_Initialize(SS, SCK, MOSI, MISO, 100);

  LCD_Disp(0x80,"E-VEHICLE CHARGE");
  LCD_Disp(0xC0," USING PV PANEL ");
  DelayMS(2500); LCDClear();

  for(unsigned char k = 0; k < 9; k++)
  {
    if(EEPROM.read(k + 0x00) == 0xFF) CardAmount[k] = 0;
    else CardAmount[k] = EEPROM.read(k + 0x00) * 10;
  }
}

void loop()
{
  LastDetectedKey = FetchKey(&KeyDebounceTime);
  
  if(ReadRFID())
  {
    unsigned char LocalVariable = RFIDRead();

    if(LocalVariable)
    {
      LCDClear();
      
      unsigned char LocalArray[5];
      memset(LocalArray, '\0', sizeof(LocalArray));
      LCD_Disp(0x80,"BALANCE:"); LCD_Decimal(0x88, CardAmount[LocalVariable - 1], 4, DEC);
      LCD_Disp(0xC0,"PERIOD:");  Keypad(0xC7, LocalArray, 4); 
      DelayMS(500); LCDClear();

      unsigned int Price = (int)(atoi(LocalArray) * 0.1667F); //PerMinute -> 10RS
      
      if(CardAmount[LocalVariable - 1] < Price)
      {
        LCD_Disp(0x80,"  LOW BALANCE  ");
        digitalWrite(Alarm, HIGH); delay(500);
        digitalWrite(Alarm, LOW); delay(100);
      }
      else
      {
        if(!ISPort1Available)
        {
          ISPort1Available = True;
          LCD_Disp(0x80,"PORT 1 IS ALLORT");
          CardAmount[LocalVariable - 1] -= Price;
          EEPROM.write(0x00 + (LocalVariable - 1), CardAmount[LocalVariable - 1] / 10);
          Port1Timer = (millis() + (atoi(LocalArray) * 1000));
        }
        else if(!ISPort2Available)
        {
          ISPort2Available = True;
          LCD_Disp(0x80,"PORT 2 IS ALLORT");
          CardAmount[LocalVariable - 1] -= Price;
          EEPROM.write(0x00 + (LocalVariable - 1),  CardAmount[LocalVariable - 1] / 10);
          Port2Timer = (millis() + (atoi(LocalArray) * 1000));
        }
        else if(!ISPort3Available)
        {
          ISPort3Available = True;
          LCD_Disp(0x80,"PORT 3 IS ALLORT");
          CardAmount[LocalVariable - 1] -= Price;
          EEPROM.write(0x00 + (LocalVariable - 1) , CardAmount[LocalVariable - 1] / 10);
          Port3Timer = (millis() + (atoi(LocalArray) * 1000));
        }
        else if(!ISPort4Available)
        {
          ISPort4Available = True;
          LCD_Disp(0x80,"PORT 4 IS ALLORT");
          CardAmount[LocalVariable - 1] -= Price;
          EEPROM.write(0x00 + (LocalVariable - 1),  CardAmount[LocalVariable - 1] / 10);
          Port4Timer = (millis() + (atoi(LocalArray) * 1000));
        }
        else
        {
          LCD_Disp(0x80,"NO PORT AVAILABL");
          LCD_Disp(0xC0,"PLS TRY LATER...");
        }
      }
    }
    else
    {
      LCD_Disp(0x80,"  INVALID CARD  ");
      LCD_Disp(0xC0, (char*)TagArray);
      digitalWrite(Alarm, HIGH); delay(1000);
    }
    delay(250); delay(250);
    digitalWrite(Alarm, LOW);
    LCDClear();
  }

  if(ISPort1Available && millis() < Port1Timer)
  {
    digitalWrite(Port1, HIGH);
    LCD_Decimal(0x80, (Port1Timer - millis()) / 1000, 5, DEC);
  }
  else if(ISPort1Available && millis() > Port1Timer)
  {
    digitalWrite(Port1, LOW);
    LCD_Disp(0x80, "     ");
    ISPort1Available = False;
  }

  if(ISPort2Available && millis() < Port2Timer)
  {
    digitalWrite(Port2, HIGH);
    LCD_Decimal(0x88, (Port2Timer - millis()) / 1000, 5, DEC);
  }
  else if(ISPort2Available && millis() > Port2Timer)
  {
    digitalWrite(Port2, LOW);
    LCD_Disp(0x88, "     ");
    ISPort2Available = False;
  }

  if(ISPort3Available && millis() < Port3Timer)
  {
    digitalWrite(Port3, HIGH);
    LCD_Decimal(0xC0, (Port3Timer - millis()) / 1000, 5, DEC);
  }
  else if(ISPort3Available && millis() > Port3Timer)
  {
    digitalWrite(Port3, LOW);
    LCD_Disp(0xC0, "     ");
    ISPort3Available = False;
  }

  if(ISPort4Available && millis() < Port4Timer)
  {
    digitalWrite(Port4, HIGH);
    LCD_Decimal(0xC8, (Port4Timer - millis()) / 1000, 5, DEC);
  }
  else if(ISPort4Available && millis() > Port4Timer)
  {
    digitalWrite(Port4, LOW);
    LCD_Disp(0xC8, "     ");
    ISPort4Available = False;
  }

  if(LastDetectedKey == SET && KeyDebounceTime < 300)
  {
    LCDClear(); LCD_Disp(0x80," CHECK BALANCE ");
    while(!ReadRFID()); 
    unsigned char LocalVariable = RFIDRead();
    LCD_Disp(0xC0,"BALANCE:"); 
    LCD_Decimal(0xC8, CardAmount[LocalVariable - 1], 4, DEC);
    DelayMS(1000); LCDClear();
  }
  if(LastDetectedKey == SET && KeyDebounceTime > 300)
  {
    unsigned int ECAmount;
    unsigned char LocalArray[5];
    LCDClear();
    LCD_Disp(0x80,"RECHARGE ROUTINE");
    while(!ReadRFID());
    unsigned char LocalVariable = RFIDRead();
    LCD_Disp(0xC0,"RECHARGE:");
    Keypad(0xC9, LocalArray, 4);
    ECAmount = atoi(LocalArray);
    EEPROM.write(LocalVariable - 1, ((CardAmount[LocalVariable - 1] + ECAmount) / 10));
    CardAmount[LocalVariable - 1] = EEPROM.read(LocalVariable - 1) * 10;
    DelayMS(500); LCDClear();
  }
}

