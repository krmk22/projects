#ifndef _ADC_H
#define _ADC_H

//128 Prescale is Maximum For Any Crystals

void ADC_Initialize();
unsigned int ReadADC(char Channel);
unsigned int ReadRawADC(char Channel, char Samples);
#endif
