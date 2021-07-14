#include <Arduino.h>
#include <avr/io.h>
#include <EEPROM.h>

#include "utils.h"
#include "i2c.h"
#include "ds1307.h"
#include "keypad.h"
#include "lcd.h"
#include "spi.h"
#include "mfrc522.h"
#include "uart.h"
#include "sim800l.h"
#include "mqtt.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define LCDData   13
#define LCDClock  12
#endif

#ifndef ObjectConfig
#define ObjectConfig
#define Lane1IR   11
#define Lane2IR   10
#define Lane3IR    9
#define Lane4IR    8
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define AlarmPin  2
#endif

#ifndef RFIDConfig
#define RFIDConfig
#define SS1      7
#define SS2      6
#define SCK      5
#define MOSI     4
#define MISO     3
#endif

#ifndef KeypadConfig
#define KeypadConfig  
#define KeySET    A0 
#define KeyMOV    A1
#define KeyENT    A2
#define KeyINC    A3
#define KeyDEC    A4
#endif

#ifndef MNumber
#define MNumber
#define MNumber1 "9791272908"
#define MNumber2 "8825959080"
#define MNumber3 "8248964523"
#define MNumber4 "8248964523"
#define MNumber5 "8248964523"
#define MNumber6 "8248964523"
#define MNumber7 "8248964523"
#define MNumber8 "8248964523" 
#define MNumber9 "8248964523"
#endif

#ifndef RFIDData
#define RFIDData
#define Card1   "BC37EE3257"
#define Card2   "60E1CA7338"
#define Card3   "49A304C32D"
#define Card4   "A7488B3450"
#define Card5   "779555D364"
#define Card6   "47F3813401"
#define Card7   "AA3A278334"
#define Card8   "990F76D535"
#define Card9   "199466D53E"
#endif

unsigned int CardAmount[10];
unsigned int KeyDebounceTime, LastReceivedData;

extern unsigned char TagArray[16], SlaveSelect;
unsigned char LastDetectedKey, LastDetectedCard;
unsigned char Hours, Minutes, Seconds, Day, Date, Month, Year;
unsigned char RegSlots, AvilSlots, CardInTime[10];
char RefVar1, RefVar2, RefVar3, RefVar4;
char ISLane1Registered, ISLane2Registered;
char ISLane3Registered, ISLane4Registered;

unsigned char RFIDRead()
{
  if(strstr(TagArray, Card1) != '\0') return 1;
  else if(strstr(TagArray, Card2) != '\0') return 2;
  else if(strstr(TagArray, Card3) != '\0') return 3;
  else if(strstr(TagArray, Card4) != '\0') return 4;
  else if(strstr(TagArray, Card5) != '\0') return 5;
  else if(strstr(TagArray, Card6) != '\0') return 6;
  else if(strstr(TagArray, Card7) != '\0') return 7;
  else if(strstr(TagArray, Card8) != '\0') return 8;
  else if(strstr(TagArray, Card9) != '\0') return 9;
  return 0;
}

unsigned char CallReadRegistration()
{
  unsigned char MobileNumber[12];
  if(GSM_Incoming(MobileNumber, DECLINE))
  {
    LCD_Disp(0xC8, "REGISTER");
    unsigned char RegisterLane;

    if(!RefVar1) RegisterLane = 1;
    else if(!RefVar2) RegisterLane = 2;
    else if(!RefVar3) RegisterLane = 3;
    else if(!RefVar4) RegisterLane = 4;

    if(RegisterLane == 1)
    {
      RefVar1 = 2; RegSlots++; ISLane1Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registred on Slot-1");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel3, RefVar1);
    }
    else if(RegisterLane == 2) 
    {
      RefVar2 = 2; RegSlots++; ISLane2Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-2");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel4, RefVar2);
    }
    else if(RegisterLane == 3)
    {
      RefVar3 = 2; RegSlots++; ISLane3Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-3");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel5, RefVar3);
    }
    else if(RegisterLane == 4)
    {
      RefVar4 = 2; RegSlots++; ISLane4Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-4");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel6, RefVar4);
    }
    else
    {
      GSM_SendMessage(MobileNumber, "We Have Filled Slots.../n Please Try Again");
      MQTT_Publish(VariableLabel2, RegSlots);
    }
    Serial_Flush(); DelayMS(500); LCDClear();
  }
}

unsigned char WEBReadRegistration()
{
  if(MQTT_Receive(VariableLabel7, &LastReceivedData))
  {
    unsigned char LocalArray[5], MobileNumber[12], RegisterLane;
    memset(LocalArray, '\0', sizeof(LocalArray));
    memset(MobileNumber, '\0', sizeof(MobileNumber));
    dtoa(LastReceivedData, LocalArray, 4, DEC);
    LCD_Disp(0xC8, "REGISTER");

    if(!RefVar1) RegisterLane = 1;
    else if(!RefVar2) RegisterLane = 2;
    else if(!RefVar3) RegisterLane = 3;
    else if(!RefVar4) RegisterLane = 4;
    else RegisterLane = 0;

    if(strstr(MNumber1, LocalArray) != '\0') strncpy(MobileNumber, MNumber1, strlen(MNumber1));
    else if(strstr(MNumber2, LocalArray) != '\0') strncpy(MobileNumber, MNumber2, strlen(MNumber2));
    else if(strstr(MNumber3, LocalArray) != '\0') strncpy(MobileNumber, MNumber3, strlen(MNumber3));
    else if(strstr(MNumber4, LocalArray) != '\0') strncpy(MobileNumber, MNumber4, strlen(MNumber4));
    else if(strstr(MNumber5, LocalArray) != '\0') strncpy(MobileNumber, MNumber5, strlen(MNumber5));
    else if(strstr(MNumber6, LocalArray) != '\0') strncpy(MobileNumber, MNumber6, strlen(MNumber6));
    else if(strstr(MNumber7, LocalArray) != '\0') strncpy(MobileNumber, MNumber7, strlen(MNumber7));
    else if(strstr(MNumber8, LocalArray) != '\0') strncpy(MobileNumber, MNumber8, strlen(MNumber8));
    else if(strstr(MNumber9, LocalArray) != '\0') strncpy(MobileNumber, MNumber9, strlen(MNumber9));
    else return False;

    if(RegisterLane == 1) 
    {
      RefVar1 = 2; RegSlots++; ISLane1Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-1");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel3, RefVar1);
    }
    else if(RegisterLane == 2) 
    {
      RefVar2 = 2; RegSlots++; ISLane2Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-2");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel4, RefVar2);
    }
    else if(RegisterLane == 3)
    {
      RefVar3 = 2; RegSlots++; ISLane3Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-3");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel5, RefVar3);
    }
    else if(RegisterLane == 4)
    {
      RefVar4 = 2; RegSlots++; ISLane4Registered = True;
      GSM_SendMessage(MobileNumber, "Your Parking Has Been Registered on Slot-4");
      MQTT_Publish(VariableLabel2, RegSlots);
      MQTT_Publish(VariableLabel6, RefVar4);
    }
    else
    {
      GSM_SendMessage(MobileNumber, "We Have Filled Slots.../n Please Try Again");
      MQTT_Publish(VariableLabel2, RegSlots);
    }
    Serial_Flush(); DelayMS(500); LCDClear();
  }
}

void setup()
{
  pinMode(AlarmPin, OUTPUT);
  digitalWrite(AlarmPin, LOW);

  pinMode(Lane1IR, INPUT);
  pinMode(Lane2IR, INPUT);
  pinMode(Lane3IR, INPUT);
  pinMode(Lane4IR, INPUT);

  LCD_Initialize(LCDClock, LCDData);
  Keypad_Initialize(KeySET, KeyMOV, KeyENT, KeyINC, KeyDEC);
  pinMode(SS1, OUTPUT); digitalWrite(SS1, HIGH);
  pinMode(SS2, OUTPUT); digitalWrite(SS2, HIGH);
  RFID_Initialize(SS1, SCK, MOSI, MISO, 50);
  RFID_Initialize(SS2, SCK, MOSI, MISO, 50);

  LCD_Disp(0x80,"IOT ENABLED CAR ");
  LCD_Disp(0xC0," PARK & SHARING ");
  DelayMS(2500); LCDClear();

  for(unsigned char k = 0; k < 9; k++)
  {
    if(EEPROM.read(k + 0x00) == 0xFF) CardAmount[k] = 0x00;
    else CardAmount[k] = EEPROM.read(k + 0x00) * 10;
  }

  if(GSM_Initialize(9600)) LCD_Disp(0x80,"GSM INITIALIZED");
  else LCD_Disp(0x80,"GSM NOT FOUND"); DelayMS(500); LCDClear();

  if(GSM_CheckSignal()) LCD_Disp(0x80,"SIM TOWERED");
  else LCD_Disp(0x80,"N/W ERROR"); DelayMS(500); LCDClear();

  if(GSM_Identifier(True)) LCD_Disp(0x80,"CALL READY");
  else LCD_Disp(0x80,"CALL ERROR"); DelayMS(500); LCDClear();

  if(GSM_EnableGPRS(True)) LCD_Disp(0x80,"GPRS READY");
  else LCD_Disp(0x80,"GPRS ERROR"); DelayMS(500); LCDClear();

  if(GSM_StartGPRS(ServerName, ServerPort)) LCD_Disp(0x80,"SERVER CONNECT");
  else LCD_Disp(0x80,"SERVER ERROR"); DelayMS(500); LCDClear();

  if(MQTT_Connect()) LCD_Disp(0x80,"MQTT CONNECTED");
  else LCD_Disp(0x80,"MQTT NT CONNECT"); DelayMS(500); LCDClear();

  MQTT_Publish(VariableLabel1, 4);
  MQTT_Publish(VariableLabel2, 0);
  MQTT_Publish(VariableLabel3, RefVar1);
  MQTT_Publish(VariableLabel4, RefVar2);
  MQTT_Publish(VariableLabel5, RefVar3);
  MQTT_Publish(VariableLabel6, RefVar4);

  AvilSlots = 4;
  if(MQTT_Subscribe(VariableLabel7, True)) LCD_Disp(0x80,"SLOTS READY");
  else LCD_Disp(0x80, "SLOTS ERROR"); DelayMS(500); LCDClear();
  Serial_Flush();
}

void loop()
{
  LastDetectedKey = FetchKey(&KeyDebounceTime);
  WEBReadRegistration(); CallReadRegistration();
  LastDetectedCard = ReadRFID();
  FetchTime(&Hours, &Minutes, &Seconds);
  FetchDate(&Day, &Date, &Month, &Year);

  LCD_Disp(0x80,"AS:"); LCD_Disp(0x85,"RS:");
  LCD_Decimal(0x83, AvilSlots, 1, DEC);
  LCD_Decimal(0x88, RegSlots, 1, DEC);

  LCD_Decimal(0x8A, Date, 2, DEC);
  LCD_Write(0x8C, '/');
  LCD_Decimal(0x8D, Month, 2, DEC);
  LCD_Disp(0x8F, " ");

  LCD_Decimal(0xC0, Hours, 2, DEC);
  LCD_Write(0xC2, ':');
  LCD_Decimal(0xC3, Minutes, 2, DEC);
  LCD_Write(0xC5, ':');
  LCD_Decimal(0xC6, Seconds, 2, DEC);
  LCD_Disp(0xC8, "       ");

  if(digitalRead(Lane1IR) && (!RefVar1 || RefVar1 == 2))
  {
    if(ISLane1Registered)
    {
      ISLane1Registered = False;
      RegSlots--;
    }
    RefVar1 = 1; AvilSlots -= 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel2, RegSlots);
    MQTT_Publish(VariableLabel3, RefVar1);
  }
  else if(!digitalRead(Lane1IR) && RefVar1 == 1)
  {
    RefVar1 = 0; AvilSlots += 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel3, RefVar1);
  }
  
  if(digitalRead(Lane2IR) && (!RefVar2 || RefVar2 == 2))
  {
    if(ISLane2Registered) 
    {
      ISLane2Registered = False;
      RegSlots--;
    }
    RefVar2 = 1; AvilSlots -= 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel2, RegSlots);
    MQTT_Publish(VariableLabel4, RefVar2);
  }
  else if(!digitalRead(Lane2IR) && RefVar2 == 1)
  {
    RefVar2 = 0; AvilSlots += 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel4, RefVar2);
  }
  
  if(digitalRead(Lane3IR) && (!RefVar3 || RefVar3 == 2))
  {
    if(ISLane3Registered)
    {
      ISLane3Registered = False;
      RegSlots--;
    }
    RefVar3 = 1; AvilSlots -= 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel2, RegSlots);
    MQTT_Publish(VariableLabel5, RefVar3);
  }
  else if(!digitalRead(Lane3IR) && RefVar3 == 1)
  {
    RefVar3 = 0; AvilSlots += 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel5, RefVar3);
  }
  
  if(digitalRead(Lane4IR) && (!RefVar4 || RefVar4 == 2))
  {
    if(ISLane4Registered)
    {
      ISLane4Registered = False;
      RegSlots--;
    }
    RefVar4 = 1; AvilSlots -= 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel2, RegSlots);
    MQTT_Publish(VariableLabel5, RefVar4);
  }
  else if(!digitalRead(Lane4IR) && RefVar4 == 1)
  {
    RefVar4 = 1; AvilSlots += 1;
    MQTT_Publish(VariableLabel1, AvilSlots);
    MQTT_Publish(VariableLabel5, RefVar4);
  }

  if(LastDetectedCard)
  {
    unsigned char LocalVariable = RFIDRead();

    if(LocalVariable && LastDetectedCard == SS1) 
    {
      CardInTime[LocalVariable - 1] = Hours;
      LCD_Disp(0xC8, "VEH IN "); DelayMS(500);
    }
    if(LocalVariable && LastDetectedCard == SS2 && CardInTime[LocalVariable - 1])
    {
      unsigned char FinalTime = Hours - CardInTime[LocalVariable - 1];
      if(!FinalTime) FinalTime = 1; LCD_Disp(0xC8, "VEH OUT ");
      int DebittedAmount = CardAmount[LocalVariable - 1] - (50 * FinalTime);

      if(CardAmount[LocalVariable - 1] >= DebittedAmount) 
      {
        EEPROM.write(LocalVariable - 1, DebittedAmount / 10);
        CardAmount[LocalVariable - 1] = EEPROM.read(LocalVariable - 1) * 10;
      }
      else if(CardAmount[LocalVariable - 1] < 50)
      {
        digitalWrite(AlarmPin, HIGH); 
        LCD_Disp(0xC0," LOW BALANCE "); DelayMS(500);
        digitalWrite(AlarmPin, LOW); 
      }

      unsigned char LocalArray[50]; memset(LocalArray, '\0', sizeof(LocalArray));
      unsigned char RefArray[10]; memset(RefArray, '\0', sizeof(RefArray));
      strcpy(LocalArray, "Thank You...\nYour Balance:");
      dtoa(EEPROM.read(LocalVariable - 1) * 10, RefArray, 4, DEC);
      strncat(LocalArray, (char*)RefArray, strlen(RefArray));

      if(LocalVariable == 1) GSM_SendMessage(MNumber1, LocalArray);
      else if(LocalVariable == 2) GSM_SendMessage(MNumber2, LocalArray);
      else if(LocalVariable == 3) GSM_SendMessage(MNumber3, LocalArray);
      else if(LocalVariable == 4) GSM_SendMessage(MNumber4, LocalArray);
      else if(LocalVariable == 5) GSM_SendMessage(MNumber5, LocalArray);
      else if(LocalVariable == 6) GSM_SendMessage(MNumber6, LocalArray);
      else if(LocalVariable == 7) GSM_SendMessage(MNumber7, LocalArray);
      else if(LocalVariable == 8) GSM_SendMessage(MNumber8, LocalArray);
      else if(LocalVariable == 9) GSM_SendMessage(MNumber9, LocalArray);
    }
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
    LCDClear(); LCD_Disp(0x80,"RECHARGE ROUTINE");
    while(!ReadRFID());
    unsigned char LocalVariable = RFIDRead();
    LCD_Disp(0xC0,"RECHARGE:");
    Keypad(0xC9, LocalArray, 4);
    ECAmount = atoi(LocalArray);
    EEPROM.write(LocalVariable - 1, ((CardAmount[LocalVariable - 1] + ECAmount) / 10));
    CardAmount[LocalVariable - 1] = EEPROM.read(LocalVariable - 1) * 10;
    DelayMS(500); LCDClear();
  }

  if(LastDetectedKey == MOV && KeyDebounceTime < 300)
  {
    LCDClear();
    LCD_Disp(0x80,"TIME:");
    LCD_Decimal(0x85, Hours, 2, DEC);
    LCD_Write(0x87, ':');
    LCD_Decimal(0x88, Minutes, 2, DEC);
    LCD_Write(0x89, ':');
    LCD_Decimal(0x8A, Seconds, 2, DEC);
    
    LCD_Disp(0xC0,"DATE:");
    LCD_Decimal(0xC5, Date, 2, DEC);
    LCD_Write(0xC7, '/');
    LCD_Decimal(0xC8, Month, 2, DEC);
    LCD_Write(0xC9, '/');
    LCD_Decimal(0xCA, Year, 2, DEC);
    DelayMS(1000); LCDClear();
  }
  if(LastDetectedKey == MOV && KeyDebounceTime > 300)
  {
    unsigned char LocalArray[10];
    LCDClear(); RTC_Keypad(LocalArray);
    UpdateTime(LocalArray[0], LocalArray[1], LocalArray[2]);
    UpdateDate(0, LocalArray[3], LocalArray[4], LocalArray[5]);
    DelayMS(500); LCDClear();
  }

  if(LastDetectedKey == ENT && KeyDebounceTime > 500)
  {
    for(unsigned char k = 0; k < 9; k++)
    {
      CardAmount[k] = 0;
      EEPROM.write(0x00 + k, CardAmount[k]);
    }
    LCD_Disp(0xC8, "FLUSHED");
  }
}
