#ifndef _ADC_H
#define _ADC_H

#if _XTAL_FREQ <= 5000000UL
#define ClockSelect 1
#define Oscillation 8
#elif _XTAL_FREQ <= 10000000UL
#define ClockSelect 5
#define Oscillation 16
#elif _XTAL_FREQ <= 16000000UL
#define ClockSelect 2
#define Oscillation 32
#elif _XTAL_FREQ <= 20000000UL
#define ClockSelect 6
#define Oscillation 64
#endif

#ifndef ADCTimingConfig
#define ADCTimingConfig
#define MinimumAcquistionTime 20 //in us
#define MinimumTadTime 2 //in us
#endif

unsigned char ReadADC8(unsigned char);
unsigned int ReadADC10(unsigned char);
unsigned char ReadRawADC8(unsigned char, unsigned char);
unsigned int ReadRawADC10(unsigned char, unsigned char);
#endif