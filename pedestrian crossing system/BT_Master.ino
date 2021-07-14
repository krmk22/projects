#include <Arduino.h>
#include <avr/io.h>
#include <SoftwareSerial.h> 

#include "utils.h"
#include "lcd.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define RS        13
#define EN        12
#define D4        11
#define D5        10
#define D6         9
#define D7         8
#endif

#ifndef MotionConfig
#define MotionConfig
#define MotionSensor  7
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm         6
#endif

#ifndef SerialConfig
#define SerialConfig
#define Transmitter   3
#define Receiver      2
#define ReqBaud       9600
#endif

#ifndef TrafficConfig
#define TrafficConfig
#define PRed       A0
#define PGreen     A1
#define TRed       A4
#define TYellow    A3
#define TGreen     A2
#endif

#ifndef UserConfig
#define UserConfig
#define VehicleTimer   2500
#define PedestTimer    5000
#define WarningTimer   2500
#define RoutineTimer   25000 //MS
#endif

unsigned long RoutineInterval;
unsigned char BTArray[50], BTCount;
char ISHumanDetectedM, ISHumanDetectedS;

SoftwareSerial BT(Receiver, Transmitter);

void BT_Receive()
{
  sei();
  while(BT.available() > 0)
  {
    delay(1);
    if(BT.available() > 0)
    {
      BTArray[BTCount] = BT.read();
      BTCount = (BTCount < 50 ?BTCount+1 :0);
    }
  }
  cli();
}

char BT_TalkBack(const char State, const char *TBString, unsigned char Length, unsigned int TimeDelay)
{
  if(State) BT.println();

  do
  {
    if(smemmem(BTArray, BTCount, TBString, Length) != '\0') return True;
    if(smemmem(BTArray, BTCount, "ERROR", 5) != '\0') return False;
    delay(1);
  }
  while(TimeDelay --> 0);
  return False;
}

void BT_Initialize()
{
  BT.print("AT"); memset(BTArray, '\0', sizeof(BTArray)); BTCount = 0;
  if(BT_TalkBack(True, "OK", 2, 2500)) LCD_Disp(0x80,"BT MODULE FOUND");
  else LCD_Disp(0x80,"BT NOT FOUND"); DelayMS(500); LCDClear();

  BT.print("AT+ROLE=1"); memset(BTArray, '\0', sizeof(BTArray)); BTCount = 0;
  if(BT_TalkBack(True, "OK", 2, 2500)) LCD_Disp(0x80,"MASTER MODE");
  else LCD_Disp(0x80,"MODE ERROR"); DelayMS(500); LCDClear();

  BT.print("AT+CMODE=0"); memset(BTArray, '\0', sizeof(BTArray)); BTCount = 0;
  if(BT_TalkBack(True, "OK", 2, 2500)) LCD_Disp(0x80,"READY TO PAIR");
  else LCD_Disp(0x80,"FAIL TO PAIR"); DelayMS(500); LCDClear();

  BT.print("AT+BIND=0020,12,080F5D"); memset(BTArray, '\0', sizeof(BTArray)); BTCount = 0;
  if(BT_TalkBack(True, "OK", 2, 5000)) LCD_Disp(0x80,"DEVICE PAIRED");
  else LCD_Disp(0x80,"PAIR ERROR"); DelayMS(500); LCDClear();
}

void setup()
{
  Serial.begin(9600);
  LCD_Initialize(RS, EN, D4, D5, D6, D7);
  BT.begin(ReqBaud);
  pinMode(MotionSensor, INPUT);
  pinMode(Alarm, OUTPUT);
  digitalWrite(Alarm, LOW);

  pinMode(PRed, OUTPUT);
  pinMode(PGreen, OUTPUT);
  pinMode(TRed, OUTPUT);
  pinMode(TYellow, OUTPUT);
  pinMode(TGreen, OUTPUT);

  digitalWrite(PRed, HIGH);
  digitalWrite(PGreen, LOW);
  digitalWrite(TRed, LOW);
  digitalWrite(TYellow, LOW);
  digitalWrite(TGreen, HIGH);

  attachInterrupt(0, BT_Receive, HIGH);
  memset(BTArray, '\0', sizeof(BTArray));
  BTCount = 0;

  LCD_Disp(0x80,"PEDESTRIAN CROSS");
  LCD_Disp(0xC0," CONTROL SYSTEM ");
  DelayMS(2500); LCDClear();
  RoutineInterval = millis() + RoutineTimer;
}

void loop()
{  
  LCD_Disp(0x80,"M:");
  LCD_Disp(0xC0,"S:");

  LCD_Disp(0x82, (ISHumanDetectedM ?"DETECT" :"NORMAL"));
  LCD_Disp(0xC2, (ISHumanDetectedS ?"DETECT" :"NORMAL"));

  LCD_Decimal(0xCE, (RoutineInterval - millis()) / 1000, 2, DEC);
  
  if(strstr((char*)BTArray, "DETECTED") != '\0' && !ISHumanDetectedS)
  {
    memset(BTArray, '\0', sizeof(BTArray));
    BTCount = 0; ISHumanDetectedS = True;
  }
  if(digitalRead(MotionSensor) && !ISHumanDetectedM)
  {
    BT.write("DETECTED");
    ISHumanDetectedM = True;
  }

  if(millis() > RoutineInterval)
  {
    if(ISHumanDetectedM || ISHumanDetectedS)
    {
      BT.write("EXECUTE");
      BT_TalkBack(False, "OK", 2, 2500);

      digitalWrite(PRed, HIGH);
      digitalWrite(PGreen, LOW);
      digitalWrite(TRed, LOW);
      digitalWrite(TYellow, HIGH);
      digitalWrite(TGreen, LOW);
      delay(VehicleTimer);

      digitalWrite(PRed, LOW);
      digitalWrite(PGreen, HIGH);
      digitalWrite(TRed, HIGH);
      digitalWrite(TYellow, LOW);
      digitalWrite(TGreen, LOW);
      delay(PedestTimer);

      digitalWrite(Alarm, HIGH);
      delay(WarningTimer);
      digitalWrite(Alarm, LOW);
      
      digitalWrite(PRed, HIGH);
      digitalWrite(PGreen, LOW);
      digitalWrite(TRed, LOW);
      digitalWrite(TYellow, LOW);
      digitalWrite(TGreen, HIGH);

      ISHumanDetectedS = False;
      ISHumanDetectedM = False;
    }
    RoutineInterval = millis() + RoutineTimer;
  }
}

