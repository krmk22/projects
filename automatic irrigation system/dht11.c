#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#ifndef _TIMER_H
#include "timer.h"
#endif

#include "dht11.h"

void DHT_Initialize()
{
	StartTimer();
	DHTDirection &=~ (1 << DHTSensor);
	DHTPort |= (1 << DHTSensor);
	__delay_ms(200);
}	

void FetchDHT(float *Var1, float *Var2)
{
	if(Millis >= DHTLastTimer)
	{
		StopTimer();
		DHTPort &=~ (1 << DHTSensor);
		__delay_ms(20);
		
		DHTDirection |= (1 << DHTSensor);
		__delay_us(40);
		
		Variable = 0; while(!DHTPin && ++Variable < MaxRoutineDelay);
		Variable = 0; while(DHTPin && ++Variable < MaxRoutineDelay);
		
		for(unsigned char ArrayShift = 0; ArrayShift < 5; ArrayShift++)
		{
			for(unsigned char RoutineShift = 0x80; RoutineShift; RoutineShift >>= 1)
			{
				Variable = 0; while(!DHTPin && ++Variable < MaxRoutineDelay); __delay_us(30);
				if(DHTPin) DHTArray[ArrayShift] |= RoutineShift;
				else DHTArray[ArrayShift] &=~ RoutineShift;
				Variable = 0; while(DHTPin && ++Variable < MaxRoutineDelay);
			}
		}
		
		unsigned char CheckSum = DHTArray[0] + DHTArray[1] +
								 DHTArray[2] + DHTArray[3];
		
				
		if(CheckSum == DHTArray[4])
		{
			LVar1 = (float)DHTArray[0];
			LVar1 += (float) ((DHTArray[1] % 10));
			LVar2 = (float)DHTArray[2];
			LVar2 += (float)((DHTArray[2] % 10));
		}
		
		DHTDirection &=~ (1 << DHTSensor);
		DHTPort |= (1 << DHTSensor);
		
		LastTicks += 30; //Minimum 24 MilliSeconds Added To the Timer
		DHTLastTimer = LastTicks + DHTUpdateInterval;	
		RestartTimer();
		
		*Var1 = LVar1;
		*Var2 = LVar2;
	}
}			