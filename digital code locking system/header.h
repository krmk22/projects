#ifndef _HEADER_H
#define _HEADER_H

#ifndef NumberSystem
#define NumberSystem
#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16
#endif

#ifndef ReturnConfig
#define ReturnConfig
#define True 1
#define False 0
#define Null '\0'
#endif

#ifndef SignSystem
#define SignSystem
#define Sign 1
#define NoSign 0
#endif

#ifndef TunedDelay
#define TunedDelay()\
{\
	_nop_();\
	_nop_();\
	_nop_();\
	_nop_();\
	_nop_();\
}
#endif

#ifndef DelayMS
#define DelayMS(Data)\
{\
	unsigned int LocalVariable;\
	unsigned int CallVariable;\
	CallVariable = Data;\
	while(CallVariable)\
	{\
		LocalVariable = 115;\
		while(LocalVariable > 0)\
		LocalVariable--;\
		CallVariable--;\
	}\
}
#endif

#ifndef DelayUS
#define DelayUS(Delay)\
{\
	unsigned int FDelay = Delay - 21;\
	TH0 = ((0xFFFF - FDelay) >> 8); TL0 = ((0xFFFF - FDelay) & 0xFF);\
	TMOD |= 0x01; ET0 = 0; TR0 = 1; while(!TF0); TR0 = 0; TF0 = 0;\
}
#endif

long MapValue(long, long, long, long, long);
//double power(double, int);
//int dtoa(unsigned long, char*, int, const char);
//int ftostra(float, int, int, char*, const char);
#endif