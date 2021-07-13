#include <at89x52.h>
#include <intrins.h>
#include "header.h"

long MapValue(long MainValue, long StartMax, long StartMin, long EndMin, long EndMax)
{
	return (MainValue - StartMin) * (EndMax - EndMin) / (StartMax - StartMin) + EndMin;
}

/*
double power(double x, int n)
{
	double result;
	int minus;
	
	result = 1;
	minus = 1;
	
	if(n < 0)
	{
		minus = -1;
		n = -n;
	}
	
	if(0 == n) return True;
	if(0 == x) return False;
	
	while(n)
	{
		if(n & 1) result *= x;
		x *= x; n /= 2;
	}
	
	if(minus < 0) return 1.0 / result;
	else return result;
}
*/

/*
int dtoa(unsigned long int Value, char *StoreArray, int EndPoint, const char Base)
{
	unsigned char CrX;
	unsigned TpT;
	
	TpT = EndPoint;
	StoreArray += EndPoint;
	if(!EndPoint) return False;
	
	do
	{
		CrX = Value % Base; Value /= Base;
		if(CrX >= 10) CrX += 'A' - '0' - 10;
		CrX += '0'; (*(--StoreArray)) = CrX;
	}
	while(--EndPoint);
	
	StoreArray += TpT;
	while(*(StoreArray)) (*(StoreArray++)) = Null;
	return True;
}
*/

/*
int ftostra(float Value, int PostFix, int PreFix, char *StoreArray, const char Sym)
{
	int ipart;
	float fpart;
	unsigned int fcon;
	
	if(Sym == Sign)
	{
		(*(StoreArray++)) = (Value < 0 ?'-' :'+');
		Value = (Value < 0 ?-Value :Value);
	}
	
	ipart = (int)Value;
	fpart = Value - (float)ipart;
	
	if(!PostFix || !PreFix) return False;
	dtoa(ipart, StoreArray, PostFix, DEC);
	StoreArray += PostFix;
	(*(StoreArray++)) = '.';
	fcon = (int)(fpart * power(10, PreFix));
	dtoa(fcon, StoreArray, PreFix, DEC);
	return True;
}
*/