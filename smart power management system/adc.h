#ifndef _ADC_H
#define _ADC_H

//128 Prescale is Maximum For Any Crystals

#define ADCBitRate 10
#define ADCMaxValue 1023

void ADC_Initialize();
unsigned int ReadADC(char Channel);
unsigned int ReadRawADC(char Channel, char Samples);
#endif
