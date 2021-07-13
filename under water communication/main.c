#include <pic.h>
#include <string.h>

#include "clock.h"
#include "header.h"
#include "lcd.h"
#include "nrf24.h"
#include "timer.h"
#include "dht11.h"

#ifndef AlarmConfig
#define AlarmConfig
#define AlarmPort PORTB
#define AlarmDirection TRISB
#define AlarmPin 7
#endif

#define Row1 RD0
#define Row2 RD1
#define Row3 RD2
#define Row4 RD3
#define Col1 RD4
#define Col2 RD5
#define Col3 RD6

const char *TAddress = "Pipe1";
const char *RAddress0 = "Pipe1";
const char *RAddress1 = "Pipe2";

float Temperature, Humidity;
unsigned char RTemperature, RHumidity, RKey;
unsigned char DetectedKey, PipeNumber; 
unsigned char SerialArray[10], SerialCount;
unsigned char LocalCount = 0;

char MatrixKeypad();

void main()
{
	TRISD = 0x70;
	
	AlarmDirection &=~ (1 << AlarmPin);
	AlarmPort &=~ (1 << AlarmPin);
	
	LCD_Initialize();
	LCD_Disp(0x80,"UNDER WATER COMM");
	LCD_Disp(0xC0,"  BASE STATION  ");
	DelayMS(2500); LCDClear();
	
	DHT_Initialize();
	StartTimer();
	
	NRF_Initialize(45);
	NRF_TXRAddress((char*)TAddress);
	
	while(1)
	{
		FetchDHT(&Humidity, &Temperature);
		DetectedKey = MatrixKeypad();
		
		SerialArray[0] = (char)Humidity;
		SerialArray[1] = (char)Temperature;
		SerialArray[2] = DetectedKey;
		SerialCount = strlen(SerialArray);
		
		if(NRF_Transmit(SerialArray, &SerialCount))
		{
			LCD_Write(0x8F, 'T');
			RHumidity = SerialArray[0];
			RTemperature = SerialArray[1];
			RKey = SerialArray[2];
			memset(SerialArray, '\0', sizeof(SerialArray));
			DelayMS(100);
		}
		
		LCD_Disp(0x80,"T:"); LCD_Decimal(0x82, RTemperature, 2, DEC);
		LCD_Disp(0x85,"H:"); LCD_Decimal(0x87, RHumidity, 2, DEC);
		LCD_Disp(0x8A,"K:"); LCD_Decimal(0x8C, RKey, 2, DEC);
		LCD_Write(0x8F, ' ');
		
		switch(RKey)
		{
			case '0':
			LCD_Disp(0xC0,"CODE 0 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '1':
			LCD_Disp(0xC0,"CODE 1 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '2':
			LCD_Disp(0xC0,"CODE 2 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
	
			case '3':
			LCD_Disp(0xC0,"CODE 3 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '4':
			LCD_Disp(0xC0,"CODE 4 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '5':
			LCD_Disp(0xC0,"CODE 5 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '6':
			LCD_Disp(0xC0,"CODE 6 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '7':
			LCD_Disp(0xC0,"CODE 7 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '8':
			LCD_Disp(0xC0,"CODE 8 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '9':
			LCD_Disp(0xC0,"CODE 9 RECEIVED");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '*':
			LCD_Disp(0xC0,"**HAND-SHAKE** ");
			AlarmPort |= (1 << AlarmPin);
			break;
			
			case '#':
			LCD_Disp(0xC0,"               ");
			AlarmPort &=~ (1 << AlarmPin);
			break;
			
			default: break;
		}	
	}	
}				

char MatrixKeypad()
{
	Row1 = 0; Row2 = 1; Row3 = 1; Row4 = 1;
	if(!Col1){while(!Col1) return '1';}
	if(!Col2){while(!Col2) return '2';}
	if(!Col3){while(!Col3) return '3';}
	
	Row1 = 1; Row2 = 0; Row3 = 1; Row4 = 1;
	if(!Col1){while(!Col1) return '4';}
	if(!Col2){while(!Col2) return '5';}
	if(!Col3){while(!Col3) return '6';}
	
	Row1 = 1; Row2 = 1; Row3 = 0; Row4 = 1;
	if(!Col1){while(!Col1) return '7';}
	if(!Col2){while(!Col2) return '8';}
	if(!Col3){while(!Col3) return '9';}
	
	Row1 = 1; Row2 = 1; Row3 = 1; Row4 = 0;
	if(!Col1){while(!Col1) return '*';}
	if(!Col2){while(!Col2) return '0';}
	if(!Col3){while(!Col3) return '#';}
	
	return '\0';
}	