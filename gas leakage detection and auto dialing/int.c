#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#include "int.h"

static void (*TIMER0_ISR)(void);
static void (*TIMER1_ISR)(void);
static void (*TIMER2_ISR)(void);
static void (*EXTINT_ISR)(void);
static void (*RXRINT_ISR)(void);
static void (*PORTCH_ISR)(void);
static void (*CP1INT_ISR)(void);
static void (*CP2INT_ISR)(void);

static void interrupt ISR(void)
{
	if(TMR0IF && TMR0IE) TIMER0_ISR();
	if(TMR1IF && TMR1IE) TIMER1_ISR();
	if(TMR2IF && TMR2IE) TIMER2_ISR();
	if(INTF && INTE) EXTINT_ISR();
	if(RCIF && RCIE) RXRINT_ISR();
	if(RBIF && RBIE) PORTCH_ISR();
	if(CCP1IF && CCP1IE) CP1INT_ISR();
	if(CCP2IF && CCP2IE) CP2INT_ISR();
}

void EnableInterrupt(const Type, void *Loop)
{
	switch(Type)
	{
		case TIMER0: TIMER0_ISR = Loop; break;
		case TIMER1: TIMER1_ISR = Loop; break;
		case TIMER2: TIMER2_ISR = Loop; break;
		case EXTINT: EXTINT_ISR = Loop; break;
		case RXRINT: RXRINT_ISR = Loop; break;
		case PORTCH: PORTCH_ISR = Loop; break;
		case CP1INT: CP1INT_ISR = Loop; break;
		case CP2INT: CP2INT_ISR = Loop; break;
		default: break;
	}
}	

void DisableInterrupt(const Type)
{
	switch(Type)
	{
		case TIMER0: TIMER0_ISR = Null; break;
		case TIMER1: TIMER1_ISR = Null; break;
		case TIMER2: TIMER2_ISR = Null; break;
		case EXTINT: EXTINT_ISR = Null; break;
		case RXRINT: RXRINT_ISR = Null; break;
		case PORTCH: PORTCH_ISR = Null; break;
		case CP1INT: CP1INT_ISR = Null; break;
		case CP2INT: CP2INT_ISR = Null; break;
		default: break;
	}
}	