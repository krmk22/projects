#include <htc.h>
#include <string.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include "sim800l.h"

unsigned char GSM_Found()
{
	Serial_Write("AT");
	Serial_Flush();
	return STalkBack(True, "OK", 2, 2500);
}

unsigned char GSM_Echo(unsigned char State)
{
	Serial_Write("ATE");
	Serial_Send(State ?'1' :'0');
	Serial_Flush();
	return STalkBack(True, "OK", 2, 2500);
}

unsigned char GSM_SimStatus()
{
	Serial_Write("AT+CSMINS?");
	Serial_Flush();
	return STalkBack(True, "0,1", 3, 2500);
}

unsigned char GSM_PinStatus()
{
	Serial_Write("AT+CPIN?");
	Serial_Flush();
	return STalkBack(True, "READY", 5, 2500);
}			

unsigned char GSM_Initialize()
{
	Serial_Initialize(9600);
	Serial_Enable(True);
	Serial_Flush();
	GSM_Found();
	
	Serial_Write("AT+CLIP=1");
	Serial_Flush();
	STalkBack(True, "OK", 2, 2500);
	
	return GSM_Echo(False);
}

unsigned char GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
	Serial_Write("AT+CMGF=1");
	Serial_Flush();
	STalkBack(True, "OK", 2, 2500);
	Serial_Write("AT+CMGS=\"+91");
	Serial_Write(MobileNumber);
	Serial_Write("\"");
	STalkBack(True, ">", 1, 2500);
	Serial_Write(SendData);
	DelayMS(50); DelayMS(50);
	Serial_Send(0x1A);
	Serial_Flush();
	return STalkBack(False, "OK", 2, 5000);
}

unsigned char GSM_ReceiveMessage(unsigned char *StoreArray)
{
	if(smemmem((char*)SerialArray, SerialCount, "+CMT:", 4) != '\0')
	{
		if(strstr((char*)SerialArray, "+91") != Null)
		{
			SplitString((char*)SerialArray, StoreArray, "+91", '"');
			return True;
		}
	}
	return False;
}

unsigned char GSM_Incoming(unsigned char *StoreArray, unsigned char Status)
{
	if(smemmem((char*)SerialArray, SerialCount, "RING", 4) != '\0')
	{
		DelayMS(25); DelayMS(25); DelayMS(25); DelayMS(25);
		
		if(strstr((char*)SerialArray, "+91") != '\0')
		{
			SplitString((char*)SerialArray, StoreArray, "+91", '"');
			
			if(Status == ATTEND)
			{
				Serial_Write("ATA");
				Serial_Flush();
				return STalkBack(True, "OK", 2, 2500);
			}
			else
			{
				Serial_Write("ATH");
				Serial_Flush();
				return STalkBack(True, "OK", 2, 2500);
			}
		}
	}
	return False;
}	

unsigned char GSM_Outgoing(const char *MobileNumber)
{
	Serial_Write("AT+CLIP=1");
	Serial_Flush();
	STalkBack(True, "OK", 2, 2500);
	
	Serial_Write("ATD+91");
	Serial_Write(MobileNumber);
	Serial_Send(';');
	STalkBack(True, "OK", 2, 2500);
	
	Serial_Write("AT+CLCC");
	Serial_Flush();
	return STalkBack(True, "OK", 2, 2500);
}

unsigned char GSM_CheckSignal()
{
	unsigned char MaximumDelay = 20;
	
	do
	{
		Serial_Write("AT+CREG?");
		Serial_Flush();
		unsigned char LocalVariable = STalkBack(True, "OK", 2, 2500);
		
		if(strstr((char*)SerialArray, "0,0") != Null) return False;
		else if(strstr((char*)SerialArray, "0,3") != Null) return False;
		else if(strstr((char*)SerialArray, "0,4") != Null) return False;
		else if(strstr((char*)SerialArray, "0,1") != Null) return True;
		else if(strstr((char*)SerialArray, "0,5") != Null) return True;
		else if(!LocalVariable) return False;
		DelayMS(1000);
	}
	
	while(MaximumDelay --> 0);
	return False;
}

unsigned char GSM_OutgoingResult()
{
	Serial_Write("AT+CLCC");
	Serial_Flush();
	STalkBack(True, "OK", 2, 2500);
	
	if(strstr((char*)SerialArray, "0,0,0,0") != '\0') return ATTEND;
	else if(strstr((char*)SerialArray, "0,6,0,0") != '\0') return DECLINE;
	else if(strstr((char*)SerialArray, "0,2,0,0") != '\0') return DIALING;
	else if(strstr((char*)SerialArray, "0,3,0,0") != '\0') return RINGING;
	return '\0';
}	

unsigned char GSM_EnableGPRS(unsigned char State)
{
	Serial_Write("AT+CGATT=");
	Serial_Send(State ?'1' :'0');
	return STalkBack(True, "+CGATT:1", 2, 5000);	
}

unsigned char GSM_EnableMUX(unsigned char State)
{
	Serial_Write("AT+CIPMUX=");
	Serial_Send(State ?'1' :'0');
	return STalkBack(True, "OK", 2, 2500);
}

unsigned char GSM_StartGPRS(const char *Server, const char *Port)
{
	Serial_Write("AT+CIPSTART=");
	Serial_Write("\"TCP\",\"");
	Serial_Write(Server);
	Serial_Write("\",");
	Serial_Write(Port);
	Serial_Flush();
	return STalkBack(True, "CONNECT", 7, 10000);
}	
							