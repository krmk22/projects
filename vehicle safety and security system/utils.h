#ifndef _UTILS_H
#define _UTILS_H

#if 0
__CONFIG(1, HSPLL);
__CONFIG(2, PWRTEN & BOREN & WDTDIS);
__CONFIG(3, MCLREN & PBDIGITAL & CCP2RC1);
__CONFIG(4, DEBUGDIS & LVPDIS & STVREN & XINSTDIS);
#endif

#ifndef _XTAL_FREQ 
#define _XTAL_FREQ 40000000UL
#endif

#ifndef ClockConfig
#define ClockConfig
#define OneMachineCycle 	0.1F
#define MachineCycleUS		10
#endif

#define DelayUS(Delay)\
{\
	unsigned int ReqDelay = Delay;\
	while(--ReqDelay) asm("nop");\
}

#ifndef DelayMS
#define DelayMS(Delay)\
{\
	register unsigned char RoutineDelay;\
	register unsigned int RequireDelay = Delay;\
	do\
	{\
		RoutineDelay = 4;\
		do\
		{\
			DelayUS(250);\
		}\
		while(--RoutineDelay);\
	}\
	while(--RequireDelay);\
}
#endif					

#ifndef NumberSystem
#define NumberSystem
#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16
#endif

#ifndef LogicalConfig
#define LogicalConfig
#define True 1
#define False 0
#define Null '\0'
#endif

#ifndef SignSystem
#define SignSystem
#define Sign 1
#define NoSign 0
#endif

extern long MapValue(long, long, long, long, long);
extern char *smemchr(register const char*, int, int);
extern char *smemrchr(register const char*, int, int);
extern void *smemmem(const void*, int, const void*, int);
extern double power(double, int);
extern int dtoa(unsigned long, unsigned char*, int, int);
extern int ftostra(float, int, int, unsigned char*, int);
extern char SplitString(unsigned char*, unsigned char*, const char*, unsigned char);
#endif