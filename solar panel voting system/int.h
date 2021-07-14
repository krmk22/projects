#ifndef _INT_H
#define _INT_H

#ifndef InterruptState
#define InterruptState
#define ActivateInterrupt() 	GIE = PEIE = 1;
#define DeactivateInterrupt()	GIE = PEIE = 0;
#endif

#define TIMER0 1
#define TIMER1 2
#define TIMER2 3
#define TIMER3 4

#define EXTIN0 5
#define EXTIN1 6
#define EXTIN2 7

#define RXRINT 8
#define PORTCH 9

#define CP1INT 10
#define CP2INT 11

void (*TIMER0_ISR)(void);
void (*TIMER1_ISR)(void);
void (*TIMER2_ISR)(void);
void (*TIMER3_ISR)(void);

void (*EXTIN0_ISR)(void);
void (*EXTIN1_ISR)(void);
void (*EXTIN2_ISR)(void);

void (*RXRINT_ISR)(void);
void (*PORTCH_ISR)(void);

void (*CP1INT_ISR)(void);
void (*CP2INT_ISR)(void);

extern void EnableInterrupt(int, void*);
extern void DisableInterrupt(int);

#endif 