//LIFI COMMUNICATION

#include <Servo.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <avr/io.h>

#include "utils.h"
#include "i2c.h"
#include "lcd.h"
#include "uart.h"
#include "spi.h"
#include "mfrc522.h"
#include "keypad.h"
#include "esp.h"
#include "mqtt.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define LCDData   13
#define LCDClock  12
#endif

#ifndef RFIDConfig
#define RFIDConfig
#define SS      11
#define SCK     10
#define MOSI     9
#define MISO     8
#define Interval 100
#endif

#ifndef ObjectConfig
#define ObjectConfig
#define ObjectSensor 7
#endif

#ifndef ServoConfig
#define ServoConfig
#define ServoMotor   6
#define DoorClose    80
#define DoorOpen     180
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm      5
#endif

#ifndef WirelessConfig
#define WirelessConfig
#define WTransmitter 3
#define WReceiver    2
#define WBaudRate    9600
#endif

#ifndef ESPConfig
#define ESPConfig
#define ESPTransmitter  1
#define ESPReceiver     0
#define ESPBaudRate     9600
#endif

#ifndef KeypadConfig
#define KeypadConfig
#define KeySET    A0
#define KeyMOV    A1
#define KeyENT    A2
#define KeyINC    A3
#define KeyDEC    A4
#endif

#ifndef RFIDDatas
#define RFIDDatas
#define CARD1   "75068821DA"
#define CARD2   "751B8221CD"
#define CARD3   "65288E21E2"
#define CARD4   "654D3B2132"
#define CARD5   "65FED32169"
#define CARD6   "750B6C2133"
#define CARD7   "65AB3521DA"
#define CARD8   "6571422177"
#define CARD9   "65CFE82163"
#define CARDA   "650FAB21E0"
#endif

extern unsigned char TagArray[16];
const char *INR = "\x1F\x04\x0E\04\x18\x08\x06\x00\x00";
unsigned char OffLineArray[25];
unsigned long ObjectAddInterval, ObjectReturnInterval;
unsigned int ShoppingAmount, CurrentAmount, KeyDebounceTime;
unsigned int StateOfRFID, ReceivedAmount;
unsigned char LastDetectedKey, DetectedCard;
char ISDoorOpen, ISObjectReturn;
char ISShopDone, ISCartOnline = False;

Servo MyServo;
SoftwareSerial MySerial(WReceiver, WTransmitter);

char RFIDRead()
{
  if(strstr((char*)TagArray, CARD1) != '\0') return 1;
  else if(strstr((char*)TagArray, CARD2) != '\0') return 2;
  else if(strstr((char*)TagArray, CARD3) != '\0') return 3;
  else if(strstr((char*)TagArray, CARD4) != '\0') return 4;
  else if(strstr((char*)TagArray, CARD5) != '\0') return 5;
  else if(strstr((char*)TagArray, CARD6) != '\0') return 6;
  else if(strstr((char*)TagArray, CARD7) != '\0') return 7;
  else if(strstr((char*)TagArray, CARD8) != '\0') return 8;
  else if(strstr((char*)TagArray, CARD9) != '\0') return 9;
  else if(strstr((char*)TagArray, CARDA) != '\0') return 10;
  else return 0;
}

void setup()
{
  pinMode(ObjectSensor, INPUT);
  pinMode(Alarm, OUTPUT);
  digitalWrite(Alarm, LOW);

  MySerial.begin(WBaudRate);
  MyServo.attach(ServoMotor);
  MyServo.write(DoorClose);

  RFID_Initialize(SS, SCK, MOSI, MISO, Interval);
  Keypad_Initialize(KeySET, KeyMOV, KeyDEC, KeyINC, KeyENT);
  LCD_Initialize(LCDClock, LCDData);

  LCD_Disp(0x80,"SMART SHOP CART ");
  LCD_Disp(0xC0,"ONLINE & OFFLINE");
  DelayMS(2500); LCDClear();

  register unsigned char LocalArray[6];
  LCD_Disp(0x80,"ENT SHOP AMOUNT ");
  LCD_CustomDisp(0xC4, 0, INR);
  Keypad(0xC0, LocalArray, 4);
  ShoppingAmount = atoi((char*)LocalArray);
  LCDClear();

  memset(OffLineArray, '\0', sizeof(OffLineArray));
  memcpy(OffLineArray, "*102030405060708090A0", 21);
  
  if(ESP_Initialize(ESPBaudRate)) LCD_Disp(0x80,"ESP MODEL FOUND...");
  else LCD_Disp(0x80,"ESP MODEL NOT FOUND"); DelayMS(500); LCDClear();

  if(ESP_Mode(3)) LCD_Disp(0x80,"ESP MODE CONFIG");
  else LCD_Disp(0x80,"ESP NOT CONFIG"); DelayMS(500); LCDClear();

  if(ESP_WiFi())
  {
    ISCartOnline = True; 
    LCD_Disp(0x80,"   ONLINE MODE  ");
    DelayMS(500); LCDClear();

    if(ESP_Start(ServerName, ServerPort)) LCD_Disp(0x80,"SERVER CONNECT");
    else LCD_Disp(0x80,"SERVER NT CONNECT"); DelayMS(500); LCDClear();

    if(MQTT_Connect()) LCD_Disp(0x80,"MQTT CONNECTED");
    else LCD_Disp(0x80,"MQTT NT CONNECT"); DelayMS(500); LCDClear();

    if(MQTT_Publish(VariableLabel1, False)) LCD_Disp(0x80,"PAGE CREATED");
    else LCD_Disp(0x80,"  PAGE ERROR  "); DelayMS(500); LCDClear();

    if(MQTT_Subscribe(VariableLabel2, True)) LCD_Disp(0x80,"CART READY");
    else LCD_Disp(0x80," CART NT READY"); DelayMS(500); LCDClear();
  }
  else LCD_Disp(0x80, " OFFLINE MODE  "); DelayMS(500); LCDClear();
  Serial_Flush();
}

void loop()
{
  if(ReadRFID()) LCD_Disp(0x80, (char*)TagArray);

  LastDetectedKey = FetchKey(&KeyDebounceTime);

  LCD_Disp(0x80, "E:");
  LCD_Decimal(0x82, ShoppingAmount, 4, DEC);
  LCD_CustomDisp(0x86, 0, INR);

  LCD_Disp(0x88, "C:");
  LCD_Decimal(0x8A, CurrentAmount, 4, DEC);
  LCD_CustomDisp(0x8E, 0, INR);

  if(ReadRFID() && !ISShopDone)
  {
    DetectedCard = RFIDRead();

    if(!ISObjectReturn)
    {
      MyServo.write(DoorOpen);
      ISDoorOpen = True;
      ObjectAddInterval = millis() + 5000;
    }
    else
    {
      MyServo.write(DoorClose);
      ISDoorOpen = False;
    }
    
    digitalWrite(Alarm, HIGH);
    delay(100); delay(100);
    digitalWrite(Alarm, LOW);
  }
  
  if(DetectedCard && millis() > ObjectAddInterval && !ISObjectReturn)
  {
    LCD_Disp(0xC0, "RESCAN LAST ITEM");
    MyServo.write(DoorClose);
    DetectedCard = False;
    ISDoorOpen = False;
  }

  if(DetectedCard && digitalRead(ObjectSensor) && !ISObjectReturn)
  {
    while(digitalRead(ObjectSensor));
    delay(350); delay(350); delay(350);
    OffLineArray[DetectedCard * 2] = '1';

    switch(DetectedCard)
    {
      case 1:
      LCD_Disp(0xC0, "BOURBON 10RS IN ");
      CurrentAmount += 10; 
      break;
      case 2:
      LCD_Disp(0xC0, "DAIRYMI 80RS IN ");
      CurrentAmount += 80; 
      break;
      case 3:
      LCD_Disp(0xC0, "FIVESTAR 5RS IN ");
      CurrentAmount += 5; 
      break;
      case 4:
      LCD_Disp(0xC0, "NABATTI 5RS IN  ");
      CurrentAmount += 5; 
      break;
      case 5:
      LCD_Disp(0xC0, "TORINO 45RS IN  ");
      CurrentAmount += 45; 
      break;
      case 6:
      LCD_Disp(0xC0, "50-50 10RS IN   ");
      CurrentAmount += 10;
      break;
      case 7:
      LCD_Disp(0xC0, "50-50 10RS IN   ");
      CurrentAmount += 10;
      break;
      case 8:
      LCD_Disp(0xC0, "OREO 10RS IN    ");
      CurrentAmount += 10; 
      break;
      case 9:
      LCD_Disp(0xC0, "BINGO 10RS IN   ");
      CurrentAmount += 10; 
      break;
      case 10:
      LCD_Disp(0xC0, "MAZZA 75RS IN   ");
      CurrentAmount += 75; 
      break;
    }

    if(CurrentAmount > ShoppingAmount)
    {
      LCD_Disp(0xC0," BUDGET REACHED ");
      digitalWrite(Alarm, HIGH);
      delay(500); delay(500);
      digitalWrite(Alarm, LOW);
    }
    MyServo.write(DoorClose);
    ISDoorOpen = False;
    DetectedCard = False;
  }

  if(LastDetectedKey == 'S' && KeyDebounceTime < 300)
  {
    LCD_Disp(0xC0,"RETURN AN OBJECT");
    MyServo.write(DoorOpen);
    ISDoorOpen = True;
    ISObjectReturn = True;
    ObjectReturnInterval = millis() + 5000;
  }

  if(ISObjectReturn && millis() > ObjectReturnInterval)
  {
    LCD_Disp(0xC0, " RETURN TIMEOUT ");
    MyServo.write(DoorClose);
    ISDoorOpen = False;
    ISObjectReturn = False;
  }

  if(ISObjectReturn && DetectedCard)
  {
    OffLineArray[DetectedCard * 2] = '0';
    
    switch(DetectedCard)
    {
      case 1:
      LCD_Disp(0xC0, "BOURBON 10RS OUT");
      CurrentAmount -= 10; 
      break;
      case 2:
      LCD_Disp(0xC0, "DAIRYMI 80RS OUT");
      CurrentAmount -= 80; 
      break;
      case 3:
      LCD_Disp(0xC0, "FIVESTAR 5RS OUT");
      CurrentAmount -= 5; 
      break;
      case 4:
      LCD_Disp(0xC0, "NABATTI 5RS OUT ");
      CurrentAmount -= 5; 
      break;
      case 5:
      LCD_Disp(0xC0, "TORINO 45RS OUT ");
      CurrentAmount -= 45; 
      break;
      case 6:
      LCD_Disp(0xC0, "50-50 10RS OUT  ");
      CurrentAmount -= 10;
      break;
      case 7:
      LCD_Disp(0xC0, "50-50 10RS OUT  ");
      CurrentAmount -= 10;
      break;
      case 8:
      LCD_Disp(0xC0, "OREO 10RS OUT   ");
      CurrentAmount -= 10; 
      break;
      case 9:
      LCD_Disp(0xC0, "BINGO 10RS OUT  ");
      CurrentAmount -= 10; 
      break;
      case 10:
      LCD_Disp(0xC0, "MAZZA 75RS OUT  ");
      CurrentAmount -= 75; 
      break;
    }
    DetectedCard = False;
    ISObjectReturn = False;
  }

  if(LastDetectedKey == 'S' && KeyDebounceTime > 300)
  {
    MyServo.write(90);
    ISShopDone = True;
    DetectedCard = False;

    if(ISCartOnline)
    {
      MQTT_Publish(VariableLabel1, CurrentAmount);
      LCD_Disp(0xC0,"PLS CLEAR AMOUNT");
    }
    else
    {
      MySerial.print((char*)OffLineArray);
      LCD_Disp(0xC0,"BILL FWD COUNTER");
    }
  }

  if(ISCartOnline && ISShopDone && MQTT_Receive(VariableLabel2, &ReceivedAmount))
  {
    CurrentAmount -= ReceivedAmount;
    if(!CurrentAmount) LCD_Disp(0xC0,"FUL AMOUNT PAID");
    else LCD_Disp(0xC0,"STILL HAS BALANCE");
  }

  if(LastDetectedKey == 'E' && KeyDebounceTime > 300)
  {
    MySerial.println("Hand Shake..!");
  }
}
