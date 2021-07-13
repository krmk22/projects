#ifndef _TIMER_H
#define _TIMER_H

/*
This Library is used to Calculate
MilliSeconds Delay. Timer0 is used
in this process. Some of Process Will
Stop The Timer Module Temporarly
*/

/*
Maximum Delay Will Be in 32Bit = 0xFFFFFFFF -> 4294967295MilliSeconds
4296967 -> Seconds; 71582.78825 -> Minutes; 1193 -> Hours; 49Days
*/ 


//TimerNumber (MaximumCycles - ((RequireDelayInSeconds * Freq) / (Prescalar * 4)

#ifndef HoldValues
#define HoldValues
#define TiMeR0 (256 - ((0.001 * _XTAL_FREQ) / (32 * 4)))
#define TiMeR1 (65536 - ((0.001 * _XTAL_FREQ) / (8 * 4)))
#define TiMeR2 (256 - ((0.001 * _XTAL_FREQ) / (16 * 4)))
#endif

#ifndef Millis
#define Millis MilliTicks
#endif

#pragma warning disable 1090
unsigned long MilliTicks = 0;

#pragma warning disable 1090
unsigned long LastTicks = 0;

void TimerRoutine();
void StartTimer();
void StopTimer();
void RestartTimer();
void ResetTimer();
#endif