#include <htc.h>

#include "int.h"

static void interrupt ISR(void)
{
	if(TMR0IE && TMR0IF) TIMER0_ISR();
	if(TMR1IE && TMR1IF) TIMER1_ISR();
	if(TMR2IE && TMR2IF) TIMER2_ISR();
	if(TMR3IE && TMR3IF) TIMER3_ISR();
	
	if(INT0IE && INT0IF) EXTIN0_ISR();
	if(INT1IE && INT1IF) EXTIN1_ISR();
	if(INT2IE && INT2IF) EXTIN2_ISR();
	
	if(RCIE && RCIF) RXRINT_ISR();
	if(RBIF && RBIE) PORTCH_ISR();
	
	if(CCP1IE && CCP1IF) CP1INT_ISR();
	if(CCP2IE && CCP2IF) CP2INT_ISR();
}	

void EnableInterrupt(int Type, void *Function)
{
	switch(Type)
	{
		case TIMER0: TIMER0_ISR = Function; break;
		case TIMER1: TIMER1_ISR = Function; break;
		case TIMER2: TIMER2_ISR = Function; break;
		case TIMER3: TIMER3_ISR = Function; break;
		
		case EXTIN0: EXTIN0_ISR = Function; break;
		case EXTIN1: EXTIN1_ISR = Function; break;
		case EXTIN2: EXTIN2_ISR = Function; break;
		
		case RXRINT: RXRINT_ISR = Function; break;
		case PORTCH: PORTCH_ISR = Function; break;
		
		case CP1INT: CP1INT_ISR = Function; break;
		case CP2INT: CP2INT_ISR = Function; break;
		
		default: break;
	}
}	

void DisableInterrupt(int Type)
{
	switch(Type)
	{
		case TIMER0: TIMER0_ISR = '\0'; break;
		case TIMER1: TIMER1_ISR = '\0'; break;
		case TIMER2: TIMER2_ISR = '\0'; break;
		case TIMER3: TIMER3_ISR = '\0'; break;
		
		case EXTIN0: EXTIN0_ISR = '\0'; break;
		case EXTIN1: EXTIN1_ISR = '\0'; break;
		case EXTIN2: EXTIN2_ISR = '\0'; break;
		
		case RXRINT: RXRINT_ISR = '\0'; break;
		case PORTCH: PORTCH_ISR = '\0'; break;
		
		case CP1INT: CP1INT_ISR = '\0'; break;
		case CP2INT: CP2INT_ISR = '\0'; break;
		
		default: break;
	}
}		