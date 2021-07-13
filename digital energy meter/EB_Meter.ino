/***********************************************************/
/****ARDUINO BASED DIGITAL EB METER WITH STATUS INFORMER****/
/***********************************************************/
#include <EEPROM.h>
#include "clock.h"
#include "header.h"
#include "eeprom.h"
#include "lcd.h"
#include "i2c.h"
#include "ds1307.h"
#include "matrix.h"
#include "uart.h"
#include "sim800l.h"

#ifndef LCDCloneConfig
#define LCDCloneConfig
#define RS A0
#define EN A1
#define D4 A2
#define D5 A3
#define D6 A4
#define D7 A5
#endif

#ifndef MatrixCloneConfig
#define MatrixCloneConfig
#define M_COL1 13
#define M_COL2 12
#define M_COL3 11
#define M_ROW1 10
#define M_ROW2 9
#define M_ROW3 8
#define M_ROW4 7
#endif

#ifndef I2CCloneConfig
#define I2CCloneConfig
#define SerialData 6
#define SerialClock 5
#endif

#ifndef EnergyCloneConfig
#define EnergyCloneConfig
#define PowerFeed 3
#endif

unsigned char LastDetectedMonth, LastDetectedKey;
unsigned char GlobalArray[100];
unsigned char MobileNumber1[12], MobileNumber2[12];
unsigned char Hour, Minute, Seconds, Day, Date, Month, Year;
volatile unsigned int KeyFetchTime, ConsumedPower;

void ChangeCurrentTime();
void EnterNumber();

void EBRoutine()
{
  if(++ConsumedPower > 10000)
  ConsumedPower = 0;
} 

void setup()
{
  pinMode(PowerFeed, INPUT); //<-- Interrupt..?

  Matrix_Initialize();
  I2C_Initialize();
  LCD_Initialize();
  LCD_Disp(0x80,"DIGITAL EB METER");
  LCD_Disp(0xC0,"WITH UNIT INFORM");
  _delay_ms(3000); LCDClear();

  Serial_Initialize();
  Serial_Enable(True);
  char LocalVariable = GSM_Initialize();
  if(LocalVariable == NO_GSM) LCD_Disp(0x80,"NO GSM MODULE");
  else if(LocalVariable == NO_SIM) LCD_Disp(0x80,"NO SIM FOUND");
  else if(LocalVariable == NO_NET) LCD_Disp(0x80,"NO NETWORK");
  _delay_ms(1000); LCDClear();
  
  FetchRTC(&Hour, &Minute, &Seconds, &Day, &Date, &Month, &Year);
  for(char k = 0; k < 10; k++)
  {
    MobileNumber1[k] = EEPROM.read(0x10 + k);
    MobileNumber2[k] = EEPROM.read(0x20 + k);
  }
  //LastDetectedMonth = (EEPROM.read(0x00) == 0xFF ?Month + 1 :EEPROM.read(0x00));
  LastDetectedMonth = Month + 1;
  attachInterrupt(1, EBRoutine, FALLING);
}

void loop()
{
  FetchRTC(&Hour, &Minute, &Seconds, &Day, &Date, &Month, &Year);
  LastDetectedKey = Matrix_FetchKey(&KeyFetchTime);

  LCD_Decimal(0x80, Hour, 2, DEC);
  LCD_Write(0x82, ':'); 
  LCD_Decimal(0x83, Minute, 2, DEC);
  LCD_Write(0x85, ':');
  LCD_Decimal(0x86, Seconds, 2, DEC);

  LCD_Decimal(0xC0, Date, 2, DEC);
  LCD_Write(0xC2, '-');
  LCD_Decimal(0xC3, Month, 2, DEC);
  LCD_Write(0xC5, '-');
  LCD_Decimal(0xC6, Year, 2, DEC);

  LCD_Decimal(0x89, ConsumedPower, 4, DEC);
  //LCD_Disp(0x8D,"UNT");
  LCD_Decimal(0xC9, ConsumedPower * 5, 4, DEC);
  //LCD_Disp(0xCD,"RS");

  if(LastDetectedMonth == Month)
  {
    unsigned char LocalArray[8];
    memset(GlobalArray, '\0', sizeof(GlobalArray) - 1);
    memset(LocalArray, '\0', sizeof(LocalArray) - 1);
    strcat(GlobalArray, "EB UNIT RESETTED...\n");
    strcat(GlobalArray, "NO: 014034 CONSUMED UNIT: ");
    dtoa(ConsumedPower, (char*)LocalArray, 5, DEC); 
    strcat(GlobalArray, (char*)LocalArray);
    strcat(GlobalArray, "\n");
    strcat(GlobalArray, "ESTIMATED BILL: ");
    dtoa(ConsumedPower * 5, (char*)LocalArray, 5, DEC); 
    strcat(GlobalArray, (char*)LocalArray);
    strcat(GlobalArray, "RS");
    ConsumedPower = 0; LCD_Disp(0x89,"SENDIN");
    GSM_SendMessage((char*)MobileNumber1, (char*)GlobalArray);
    GSM_SendMessage((char*)MobileNumber2, (char*)GlobalArray);
    LCD_Disp(0x89,"SENDED");
    if(Month > 12) Month = 0; eeprom_write(0x00, Month + 1);
    LastDetectedMonth = eeprom_read(0x00);
  }
  if(LastDetectedKey == '*' && KeyFetchTime < 300)
  {
    ChangeCurrentTime();
  }
  if(LastDetectedKey == '#' && KeyFetchTime < 300)
  {
    LCDClear();
    LCD_Disp(0x83, (char*)MobileNumber1);
    LCD_Disp(0xC3, (char*)MobileNumber2);
    _delay_ms(2500); LCDClear();
  }
  if(LastDetectedKey == '#' && KeyFetchTime > 500)
  {
    EnterNumber();
  }
}

void EnterNumber()
{
  unsigned char RequireDigit = 10;
  unsigned char MaximumMove, Variable;
  volatile unsigned int KeyFetchTime;
  
  LCDClear();
  LCD_Command(0x0E);
  LCD_Disp(0x80,"ENTER USR NUMBER");
  do
  {
    Variable = Matrix_FetchKey(&KeyFetchTime);

    if(isdigit(Variable))
    {
      MobileNumber1[MaximumMove] = Variable;
      LCD_Write(0xC0 + MaximumMove, Variable);
      if(++MaximumMove > (RequireDigit - 1))
      MaximumMove = 0;
    }
    if(Variable == '*' && KeyFetchTime < 300)
    {
      if(--MaximumMove > (RequireDigit - 1))
      MaximumMove = (RequireDigit - 1);
      LCD_Write(0xC0 + MaximumMove, ' ');
    }
    if(Variable == '*' && KeyFetchTime > 500)
    {
      LCD_Disp(0xC0,"                ");
      memset(MobileNumber1, '\0', sizeof(MobileNumber1));
      MaximumMove = 0;
    }
  }
  while(Variable != '#');
  
  LCDClear();
  LCD_Disp(0x80,"ENTER EB NUMBER ");
  MaximumMove = 0; KeyFetchTime = 0;
  do
  {
    Variable = Matrix_FetchKey(&KeyFetchTime);
    if(isdigit(Variable))
    {
      MobileNumber2[MaximumMove] = Variable;
      LCD_Write(0xC0 + MaximumMove, Variable);
      if(++MaximumMove > (RequireDigit - 1))
      MaximumMove = 0;
    }
    if(Variable == '*' && KeyFetchTime < 300)
    {
      if(--MaximumMove > (RequireDigit - 1))
      MaximumMove = (RequireDigit - 1);
      LCD_Write(0xC0 + MaximumMove, ' ');
    }
    if(Variable == '*' && KeyFetchTime > 500)
    {
      LCD_Disp(0xC0,"                ");
      memset(MobileNumber2, '\0', sizeof(MobileNumber1));
      MaximumMove = 0;
    }
  }
  while(Variable != '#');
  eeprom_writeln(0x10, (char*)MobileNumber1, 10);
  eeprom_writeln(0x20, (char*)MobileNumber2, 10);
  LCD_Command(0x0C);
  LCDClear();
}

void ChangeCurrentTime()
{
  LCD_Command(0x0E);
  unsigned char Variable;
  volatile unsigned int KeyFetchTime;
  unsigned char LocalArray[3];
  unsigned char GlobalArray[6];
  unsigned char LCDAddress = 0x80;
  unsigned char RequireDigit = 2;
  unsigned char MaximumMove;

  memset(GlobalArray, '\0', sizeof(GlobalArray));
  memset(LocalArray, '\0', sizeof(LocalArray));
  
  do
  {
    Variable = Matrix_FetchKey(&KeyFetchTime);

    if(isdigit(Variable))
    {
      LocalArray[MaximumMove] = Variable;
      LCD_Write(LCDAddress + MaximumMove, Variable);
      if(++MaximumMove > (RequireDigit - 1))
      MaximumMove = 0; delay(250);
    }
    if(Variable == '*')
    {
      if(LCDAddress == 0x80) 
      {
        LCDAddress = 0x83; 
        unsigned char LocalVariable = atoi((char*)LocalArray);
        GlobalArray[0] = (LocalVariable > 23 ?0 :LocalVariable);
      }
      else if(LCDAddress == 0x83) 
      {
        LCDAddress = 0x86; 
        unsigned char LocalVariable = atoi((char*)LocalArray);
        GlobalArray[1] = (LocalVariable > 59 ?0 :LocalVariable);
      }
      else if(LCDAddress == 0x86) 
      {
        LCDAddress = 0xC0;
        unsigned char LocalVariable = atoi((char*)LocalArray);
        GlobalArray[2] = (LocalVariable > 59 ?0 :LocalVariable);
      }
      else if(LCDAddress == 0xC0) 
      {
        LCDAddress = 0xC3;
        unsigned char LocalVariable = atoi((char*)LocalArray);
        GlobalArray[3] = (LocalVariable > 31 ?1 :LocalVariable);
      }
      else if(LCDAddress == 0xC3) 
      {
        LCDAddress = 0xC6;
        unsigned char LocalVariable = atoi((char*)LocalArray);
        GlobalArray[4] = (LocalVariable > 12 ?1 :LocalVariable);
      }
      else if(LCDAddress == 0xC6) 
      {
        LCDAddress = 0x80;
      }
      delay(250);
    }
  }
  while(Variable != '#');
  unsigned char LocalVariable = atoi((char*)LocalArray);
  GlobalArray[5] = LocalVariable;
  UpdateRTC(GlobalArray[0], GlobalArray[1], GlobalArray[2], 0, GlobalArray[3], GlobalArray[4], GlobalArray[5]);
  LCD_Command(0x0C);
}
