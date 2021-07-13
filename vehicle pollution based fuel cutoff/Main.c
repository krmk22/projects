#include <avr/io.h>
#include <string.h>

#include "clock.h"
#include "header.h"
#include "adc.h"
#include "eeprom.h"
#include "lcd.h"
#include "i2c.h"
#include "ds1307.h"
#include "pwm.h"
#include "sim800l.h"

#include <util/delay.h>

//Analog Input Configurations
#define Accelerator 0

//Alarm Configurations
#define AlarmPort PORTD
#define AlarmDirection DDRD
#define AlarmPin PIND
#define Alarm 7 

//Smoke Sensor Configuration
#define SmokeSensorPort PORTC
#define SmokeSensorDirection DDRC
#define SmokeSensorPin PINC
#define SmokeSensor PC1

//Keypad Configurations
#define SetKey (PINB & (1 << PB0) ?1 :0)
#define MovKey (PINB & (1 << PB1) ?1 :0)
#define IncKey (PINB & (1 << PB2) ?1 :0)
#define DecKey (PINB & (1 << PB5) ?1 :0)
#define EntKey (PINC & (1 << PC2) ?1 :0)

unsigned int Acceleration;
unsigned char CurrentPulse = 255;
unsigned char PollutionSensor, PollutionLevel;
unsigned char C_Hour, C_Minute, C_Seconds;
unsigned char C_Date, C_Month, C_Year, C_Day;
unsigned char E_Date, E_Month, E_Year, E_Day;
unsigned char PollutionWarning, ISSmokeExceed;
unsigned char LastDateWarning, ExpireWarning;

unsigned char MobileNumber1[12];
unsigned char MobileNumber2[12];
unsigned char MobileNumber3[12];
unsigned char GlobalArray[10];

//Routine For Threshold & Number Set
void Keypad1(unsigned char*, unsigned int);
void Keypad2(unsigned char*);

int main()
{
	AlarmDirection |= (1 << Alarm);
	AlarmPort &=~ (1 << Alarm);
	SmokeSensorDirection &=~ (1 << SmokeSensor);

	DDRB &=~ ((1 << PB0)|(1 << PB1)|(1 << PB2)|(1 << PB5));
	DDRC &=~ (1 << PC2);

	PORTB |= ((1 << PB0)|(1 << PB1)|(1 << PB2)|(1 << PB5));
	PORTC |= (1 << PC2);

	I2C_Initialize();
	ADC_Initialize();
	
	LCD_Initialize();
	LCD_Disp(0x80,"AIR POLL AND INS");
	LCD_Disp(0xC0,"BASE VEHICL LOCK");
	DelayMS(3000); LCDClear();

	eeprom_readln(0x10, (unsigned char*)MobileNumber1, 10);
	eeprom_readln(0x20, (unsigned char*)MobileNumber2, 10);

	LCD_Disp(0x80, (char*)MobileNumber1);
	LCD_Disp(0xC0, (char*)MobileNumber2);
	DelayMS(1000); LCDClear();

	E_Date = (eeprom_read(0x01) == 0xFF ?30 :eeprom_read(0x01));
	E_Month = (eeprom_read(0x02) == 0xFF ?12 :eeprom_read(0x02));
	E_Year = (eeprom_read(0x03) == 0xFF ?99 :eeprom_read(0x03));

	ExpireWarning = False;
	LastDateWarning = False;
	ISSmokeExceed = False;

	GSM_Initialize();

	while(1)
	{
		FetchDate(&C_Day, &C_Date, &C_Month, &C_Year);
		FetchTime(&C_Hour, &C_Minute, &C_Seconds);
		Acceleration = ReadRawADC(Accelerator, 10);
		PulseWidthModulation1A((unsigned char)MapValue(Acceleration, 0, 1023, 0, CurrentPulse));
		char ReceiveStatus = GSM_Incoming((unsigned char*)MobileNumber3, DECLINE);

		LCD_Decimal(0xC0, C_Hour, 2, DEC); LCD_Write(0xC2, ':');
		LCD_Decimal(0xC3, C_Minute, 2, DEC); LCD_Write(0xC5, ':');
		LCD_Decimal(0xC6, C_Seconds, 2, DEC);
		 
		LCD_Decimal(0xCA, C_Date, 2, DEC); LCD_Write(0xCC, '-');
		LCD_Decimal(0xCD, C_Month, 2, DEC); 
		LCD_Disp(0x80,"SMOKE:"); LCD_Decimal(0x8F, PollutionWarning, 1, DEC);
		
		
		if((SmokeSensorPin & (1 << SmokeSensor) ?1 :0) && !ISSmokeExceed) LCD_Disp(0x86,"NORMAL");
		if(!(SmokeSensorPin & (1 << SmokeSensor) ?1 :0) && !ISSmokeExceed)
		{
			PollutionWarning++;
			ISSmokeExceed = True;
			LCD_Disp(0x86,"SENSE ");
			AlarmPort |= (1 << Alarm);
			if(PollutionWarning > 2)
			{
				if(CurrentPulse > 100) CurrentPulse = 100;
				GSM_SendMessage((char*)MobileNumber2, "TN-59 AB 4569 Vehicle Pollution Warning Exceed");
				GSM_SendMessage((char*)MobileNumber1, "Pollution Warning Limit Exceed Vehicle Speed Reduced");
			}
			else GSM_SendMessage((char*)MobileNumber1, "Pollution Level Exceed");
			AlarmPort &=~ (1 << Alarm);
		}
		if((SmokeSensorPin & (1 << SmokeSensor) ?1 :0) && ISSmokeExceed)
		{
			LCD_Disp(0x86, "NORMAL");
			ISSmokeExceed = False;
			GSM_SendMessage((char*)MobileNumber1, "Pollution Back To Normal");
		}
		/*
		if(ReceiveStatus && PollutionWarning > 2)
		{
			if(smemmem((char*)MobileNumber2, strlen((char*)MobileNumber2), (char*)MobileNumber3, strlen((char*)MobileNumber3)) != '\0')
			{
				PollutionWarning = 0;
				LCD_Decimal(0x8F, PollutionWarning, 1, DEC);
				CurrentPulse = 255;
				ReceiveStatus = False;
			}
		}
		*/
		if(ReceiveStatus && PollutionWarning > 2)
		{
			if(strncmp((char*)MobileNumber2, strlen((char*)MobileNumber3), strlen((char*)MobileNumber2)) != '\0')
			{
				LCDClear();
				PollutionWarning = 0;
				LCD_Decimal(0x8F, PollutionWarning, 1, DEC);
				CurrentPulse = 255;
				ReceiveStatus = False;
			}
		}
		if(C_Date == E_Date && C_Month == E_Month && C_Year == E_Year && !LastDateWarning)
		{
			AlarmPort |= (1 << Alarm);
			GSM_SendMessage((char*)MobileNumber1, "Final Due Date To Renewal Vehicle Insurance");
			AlarmPort &=~ (1 << Alarm);
			LastDateWarning = True;
		}
		if((C_Date > E_Date && C_Month >= E_Month && C_Year >= E_Year) && !ExpireWarning)
		{
			AlarmPort |= (1 << Alarm);
			GSM_SendMessage((char*)MobileNumber1, "Your Vehicle Insurance is OutDated");
			GSM_SendMessage((char*)MobileNumber2, "TN-59 AB 4569 Fail to Renew Insurance");
			AlarmPort &=~ (1 << Alarm);
			ExpireWarning = True;
			CurrentPulse = 50;
		}
		if(!IncKey)
		{
			LCD_Disp(0x80,"                ");
			LCD_Disp(0xC0,"                ");
			LCD_Disp(0x80, (char*)MobileNumber1);
			LCD_Disp(0xC0, (char*)MobileNumber2);
			while(!IncKey);
			LCD_Disp(0x80,"                ");
			LCD_Disp(0xC0,"                ");
		}
		if(!DecKey)
		{
			LCD_Disp(0x80,"                ");
			LCD_Disp(0xC0,"                ");
			LCD_Disp(0x80,"E_DATE:"); LCD_Decimal(0x87, E_Date, 2, DEC);
			LCD_Write(0x89, ':'); LCD_Decimal(0x8A, E_Month, 2, DEC);
			LCD_Write(0x8C, ':'); LCD_Decimal(0x8D, E_Year, 2, DEC);
			LCD_Disp(0xC0,"DATE:"); LCD_Decimal(0xC5, C_Date, 2, DEC);
			LCD_Write(0xC7, '/'); LCD_Decimal(0xC8, C_Month, 2, DEC);
			LCD_Write(0xCA, '/'); LCD_Decimal(0xCB, C_Year, 2, DEC);
			while(!DecKey);
			LCD_Disp(0x80,"                ");
			LCD_Disp(0xC0,"                ");
		}
		if(!SetKey)
		{
			while(!SetKey);
			LCD_Disp(0x80,"SET USER NUMBER ");
			Keypad1((unsigned char*)MobileNumber1, 10);
			eeprom_writeln(0x10, (char*)MobileNumber1, 10);
			
			LCD_Disp(0x80,"SET RTO NUMBER  ");
			Keypad1((unsigned char*)MobileNumber2, 10);
			eeprom_writeln(0x20, (char*)MobileNumber2, 10);
			LCD_Disp(0x80,"                ");
			LCD_Disp(0xC0,"                ");
		}
		if(!MovKey)
		{
			while(!MovKey);
			Keypad2((unsigned char*)GlobalArray);
			LCD_Decimal(0x80, GlobalArray[0], 2, DEC);
			LCD_Decimal(0x82, GlobalArray[1], 2, DEC);
			LCD_Decimal(0x84, GlobalArray[2], 2, DEC);
			LCD_Decimal(0xC0, GlobalArray[3], 2, DEC);
			LCD_Decimal(0xC2, GlobalArray[4], 2, DEC);
			LCD_Decimal(0xC4, GlobalArray[5], 2, DEC);
			UpdateRTC(GlobalArray[0], GlobalArray[1], GlobalArray[2], 3, GlobalArray[3], GlobalArray[4], GlobalArray[5]);
		}
		if(!EntKey)
		{
			while(!EntKey); CurrentPulse = 255;
			Keypad2((unsigned char*)GlobalArray);
			eeprom_write(0x01, GlobalArray[3]);
			eeprom_write(0x02, GlobalArray[4]);
			eeprom_write(0x03, GlobalArray[5]);
			E_Date = GlobalArray[3];
			E_Month = GlobalArray[4];
			E_Year = GlobalArray[5];
		}
	}
	return '\0';
}

void Keypad1(unsigned char *RequireArray, unsigned int RequireDigit)
{
	LCD_Disp(0xC0,"                ");

	unsigned char MaximumMove = 0, InnerCount = 0;
	LCD_Command(0x0E);
	
	while(EntKey)
	{
		if(!IncKey)
		{
			while(!IncKey);
			if(++InnerCount > 9) InnerCount = 0;
		}
		if(!DecKey)
		{
			while(!DecKey);
			if(--InnerCount > 9) InnerCount = 9;
		}
		if(!MovKey)
		{
			while(!MovKey);
			if(++MaximumMove > (RequireDigit - 1)) MaximumMove = 0;
		}
		if(!SetKey)
		{
			LCD_Write(0xC0 + MaximumMove, ' ');
			while(!SetKey);
			if(--MaximumMove > (RequireDigit - 1)) MaximumMove = (RequireDigit - 1);
		}	
		RequireArray[MaximumMove] = InnerCount + '0';
		LCD_Write(0xC0 + MaximumMove, RequireArray[MaximumMove]);
	}			
	while(!EntKey);
	LCD_Command(0x0C);	
}

void Keypad2(unsigned char *StoreArray)
{
	LCD_Disp(0x80,"                ");
	LCD_Disp(0xC0,"                ");

	unsigned char KeyCount = 0;
	unsigned char MovCount = 0;
	unsigned char LCDAddress = 0x85;

	LCD_Disp(0x80,"TIME:"); 
	LCD_Write(0x87, ':');
	LCD_Write(0x8A, ':');
	LCD_Disp(0xC0,"DATE:");
	LCD_Write(0xC7, ':');
	LCD_Write(0xCA, ':');

	while(EntKey)
	{
		if(!IncKey)
		{
			while(!IncKey);
			KeyCount++;
		}
		else if(!DecKey)
		{
			while(!DecKey);
			KeyCount--;
		}
		else if(!MovKey)
		{
			while(!MovKey);
			MovCount++;
		}
		
		if(MovCount == 0)
		{
			LCDAddress = 0x85;
			if(KeyCount > 23) KeyCount = 0;
		}
		else if(MovCount == 1)
		{
			LCDAddress = 0x88;
			if(KeyCount > 59) KeyCount = 0;
		}
		else if(MovCount == 2)
		{
			LCDAddress = 0x8B;
			if(KeyCount > 59) KeyCount = 0;
		}
		else if(MovCount == 3)
		{
			LCDAddress = 0xC5;
			if(KeyCount > 31) KeyCount = 0;
		}
		else if(MovCount == 4)
		{
			LCDAddress = 0xC8;
			if(KeyCount > 12) KeyCount = 0;
		}
		else if(MovCount == 5)
		{
			LCDAddress = 0xCB;
			if(KeyCount > 99) KeyCount = 0;
		}
		else if(MovCount > 5) MovCount = 0;
		
		StoreArray[MovCount] = KeyCount;
		LCD_Write(LCDAddress, StoreArray[MovCount] / 10 + 0x30);
		LCD_Write(LCDAddress + 1, StoreArray[MovCount] % 10 + 0x30);
	}
	while(!EntKey);
	LCDClear();
}





