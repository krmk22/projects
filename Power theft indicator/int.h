#ifndef _INT_H
#define _INT_H

/*
Note Down Low End PIC Microcontroller
Has Only One Address For The Interrupt
We can't Split Interrupts Individually
By using This Call Function The Interrupt
Will Be Used To Desire Routine
Warning: Avoid More than One Function Call
It'll Result With Hardware Stack OverFlow
*/

#ifndef TypeOfInterrupts
#define TypeOfInterrupts
#define TIMER0 0
#define TIMER1 1
#define TIMER2 2
#define EXTINT 3
#define RXRINT 4
#define PORTCH 5
#define CP1INT 6
#define CP2INT 7
#endif

void EnableInterrupt(const, void*);
void DisableInterrupt(const);
#endif