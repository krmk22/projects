#include <htc.h>
#include <string.h>

#ifndef _GPIO_H
#include "gpio.h"
#endif

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _SUART_H
#include "suart.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif

#include "r307.h"

char TouchPin;

void FP_Initialize(unsigned long ReqBaud, char Pin)
{
	TouchPin = Pin;
	TRIS_INPUT(TouchPin);
	SSerial_Initialize(ReqBaud);
	SSerial_Enable(True);
	SSerial_Flush();
}

char FP_EnrollMent(unsigned char FingerID)
{
	unsigned char CheckSum = 15 + FingerID;
	
	if(CollectFingerImage()) LCD_Disp(0xC0,"FINGER CAPTURED");
	else return False; DelayMS(500); LCDClear();
	
	if(ConvertImage2Char(1)) LCD_Disp(0xC0,"FINGER STORED");
	else return False; DelayMS(500); LCDClear();
	
	if(CollectFingerImage()) LCD_Disp(0xC0,"FINGER CAPTURED");
	else return False; DelayMS(500); LCDClear();
	
	if(ConvertImage2Char(2)) LCD_Disp(0xC0,"FINGER STORED");
	else return False; DelayMS(500); LCDClear();
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\05\x00\x09", 12);
	if(SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
	LCD_Disp(0xC0,"FINGER COMBINED"); else return False; DelayMS(500); LCDClear();
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x06\x02", 11);
	SSerial_Send(0x00); SSerial_Send(FingerID); SSerial_Send(0x00); SSerial_Send(CheckSum);
	if(SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
	LCD_Disp(0xC0,"FINGER STORED"); else return False; DelayMS(500); LCDClear();
	
	return True;
}

char FP_Identify(unsigned char *FingerID, unsigned char *MatchScore)
{
	if(CollectFingerImage()) LCD_Disp(0xC0,"FINGER CAPTURED");
	else return False; DelayMS(500); LCDClear();
	
	if(ConvertImage2Char(1)) LCD_Disp(0xC0,"FINGER STORED");
	else return False; DelayMS(500); LCDClear();
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x08\x1B", 10);
	SSerial_Writeln("\x01\x00\x00\x01\x01\x00\x27", 7);
	if(SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x07\x00", 10, 2500))
	{
		DelayMS(100);
		*MatchScore = SSerialArray[13];
		*FingerID = SSerialArray[11];
		LCD_Disp(0xC0,"FINGER MATCHED");
	}
	else return False; DelayMS(500); LCDClear();
	
	return True;
}

char FP_Verify(unsigned char FingerID, unsigned char *MatchScore)
{
	unsigned char CheckSum = FingerID + 16;
	
	if(CollectFingerImage()) LCD_Disp(0xC0,"FINGER CAPTURED");
	else return False; DelayMS(500); LCDClear();
	
	if(ConvertImage2Char(1)) LCD_Disp(0xC0,"FINGER CONVERTED");
	else return False; DelayMS(500); LCDClear();	
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x07\x02", 11);
	SSerial_Send(0x00); SSerial_Send(FingerID); SSerial_Send(0x00); SSerial_Send(CheckSum);
	if(SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
	LCD_Disp(0xC0,"FINGER READER"); else return False; DelayMS(500); LCDClear();
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x03\x00\x07", 12);
	if(SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x05\x00", 10, 2500))
	{
		DelayMS(100);
		*MatchScore = SSerialArray[11];
		LCD_Disp(0xC0,"FINGER MATCHED");
	}
	else return False; DelayMS(500); LCDClear();
	
	return True;
}

char FP_Delete(unsigned char FingerID)
{
	unsigned char CheckSum = 21 + FingerID;
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x07\x0C", 10);
	SSerial_Send(0x00); SSerial_Send(FingerID); 
	SSerial_Writeln("\x00\x01\x00",3); SSerial_Send(CheckSum);
	return SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500);
}

char FP_DeleteAll()
{
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x0D\x00\x11", 12);
	return SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500);
}

char ConvertImage2Char(unsigned char PageID)
{
	unsigned int CheckSum = PageID + 7;
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x02", 10);
	SSerial_Send(PageID); SSerial_Send(0x00); SSerial_Send(CheckSum);
	return SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\07\x00\x03\x00", 10, 2500);
}

char CollectFingerImage()
{
	unsigned char ReturnVariable = False;
	
	LCD_Disp(0x80," PUT U R FINGER ");
	while(PORT_READ(TouchPin));		
	DelayMS(100); DelayMS(100);
	
	SSerial_Flush();
	SSerial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x01\x00\x05", 12);
	ReturnVariable = SSTalkBack(False, "\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500);
	
	LCD_Disp(0x80,"REMOVE UR FINGER "); 
	while(!PORT_READ(TouchPin));
	return ReturnVariable;
}	