#include <Arduino.h>
#include <avr/io.h>
#include <Servo.h>

#include "utils.h"
#include "lcd.h"
#include "adc.h"
#include "uart.h"
#include "esp.h"
#include "mqtt.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define RS 13
#define EN 12
#define D4 11
#define D5 10
#define D6  9
#define D7  8
#endif

#ifndef SensorConfig
#define SensorConfig
#define LightSensor 0
#define MotionSensor A1
#endif

#ifndef OutputConfig
#define OutputConfig
#define LampRelay 6
#define FanRelay 7
#endif

#ifndef ServoConfig
#define ServoConfig
#define ServoMotor 5
#define DoorClose  90
#define DoorOpen  180
#endif

#ifndef ESPConfig
#define ESPConfig
#define ESPTransmitter 1
#define ESPReceiver 0
#define ESPBaudRate 9600
#endif

Servo MyServo;
unsigned char LightLevel, ISDataReceived, ISAutoMode = True;
unsigned int LastReceivedData;
char ISLampOn, ISFanOn;

void setup()
{
  pinMode(LampRelay, OUTPUT);
  pinMode(FanRelay, OUTPUT);
  digitalWrite(LampRelay, LOW);
  digitalWrite(FanRelay, LOW);

  MyServo.attach(ServoMotor);
  MyServo.write(DoorClose);
  pinMode(MotionSensor, INPUT);

  ADC_Initialize();
  LCD_Initialize(RS, EN, D4, D5, D6, D7);
  LCD_Disp(0x80,"SMART HOME AUTO ");
  LCD_Disp(0xC0,"SECURITY SYSTEM ");
  DelayMS(2500); LCDClear();

  if(ESP_Initialize(ESPBaudRate)) LCD_Disp(0x80,"ESP FOUND...");
  else LCD_Disp(0x80,"ESP NOT FOUND"); DelayMS(500); LCDClear();

  if(ESP_Mode(3)) LCD_Disp(0x80,"MODE CONFIG");
  else LCD_Disp(0x80,"MODE ERROR"); DelayMS(500); LCDClear();

  if(ESP_WiFi()) LCD_Disp(0x80,"WIFI CONNECTED");
  else LCD_Disp(0x80,"WIFI ERROR"); DelayMS(500); LCDClear();

  if(ESP_Start(ServerName, ServerPort)) LCD_Disp(0x80,"SERVER CONNECTED");
  else LCD_Disp(0x80,"SERVER ERROR"); DelayMS(500); LCDClear();

  if(MQTT_Connect()) LCD_Disp(0x80,"MQTT ESTABLISHED");
  else LCD_Disp(0x80,"MQTT FAILED"); DelayMS(500); LCDClear();

  MQTT_Publish(VariableLabel1, False);
  MQTT_Publish(VariableLabel2, False);
  MQTT_Publish(VariableLabel3, False);
  MQTT_Subscribe(VariableLabel4, True); 
  DelayMS(500); Serial_Flush();
}

void loop()
{
  ISDataReceived = MQTT_Receive(VariableLabel4, &LastReceivedData);
  LightLevel = MapValue(ReadRawADC(LightSensor, 10), 0, 1023, 0, 100);

  LCD_Disp(0x80, (ISAutoMode ?"***AUTO-MODE*** " :"**MANUAL-MODE** "));
  LCD_Disp(0xC0,"L:"); LCD_Decimal(0xC2, LightLevel, 3, DEC);
  LCD_Disp(0xC6,"M:"); LCD_Disp(0xC8, (digitalRead(MotionSensor) ?"DETECT" :"NORMAL"));

  if(ISAutoMode && LightLevel < 50 && !ISLampOn)
  {
    ISLampOn = True;
    digitalWrite(LampRelay, HIGH);
    MQTT_Publish(VariableLabel1, ISLampOn);
    Serial_Flush();
  }
  else if(ISAutoMode && LightLevel > 90 && ISLampOn)
  {
    ISLampOn = False;
    digitalWrite(LampRelay, LOW);
    MQTT_Publish(VariableLabel1, ISLampOn);
    Serial_Flush();
  }

  if(ISAutoMode && digitalRead(MotionSensor) && !ISFanOn)
  {
    ISFanOn = True;
    digitalWrite(FanRelay, HIGH);
    MQTT_Publish(VariableLabel2, ISFanOn);
    Serial_Flush();
  }
  else if(ISAutoMode && !digitalRead(MotionSensor) && ISFanOn)
  {
    ISFanOn = False;
    digitalWrite(FanRelay, LOW);
    MQTT_Publish(VariableLabel2, ISFanOn);
    Serial_Flush();
  }

  if(ISDataReceived)
  {
    switch(LastReceivedData)
    {
      case 1:
      if(ISAutoMode) ISAutoMode = False;
      break;
      case 2:
      if(!ISAutoMode) ISAutoMode = True;
      break;
      case 3:
      if(!ISLampOn) ISLampOn = True;
      digitalWrite(LampRelay, HIGH);
      MQTT_Publish(VariableLabel1, ISLampOn);
      break;
      case 4:
      if(ISLampOn) ISLampOn = False;
      digitalWrite(LampRelay, LOW);
      MQTT_Publish(VariableLabel1, ISLampOn);
      break;
      case 5:
      if(!ISFanOn) ISFanOn = True;
      digitalWrite(FanRelay, HIGH);
      MQTT_Publish(VariableLabel2, ISFanOn);
      break;
      case 6:
      if(ISFanOn) ISFanOn = False;
      digitalWrite(FanRelay, LOW);
      MQTT_Publish(VariableLabel2, ISFanOn);
      break;
      case 7:
      MyServo.write(DoorOpen);
      MQTT_Publish(VariableLabel3, True);
      break;
      case 8:
      MyServo.write(DoorClose);
      MQTT_Publish(VariableLabel3, False);
      break;
      default: break;
    }
    Serial_Flush();
  }
}

