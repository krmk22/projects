#ifndef _DHT11_H
#define _DHT11_H

#ifndef DHTTimingConfig
#define DHTTimingConfig
#define DHTUpdateInterval 1000 //in MilliSeconds
#define MaxRoutineDelay 100
#endif

#ifndef DHTConfig
#define DHTConfig
#define DHTPort PORTD
#define DHTDirection TRISD
#define DHTSensor 0
#endif

static bit DHTPin @ ((unsigned char) &DHTPort * 8 + DHTSensor);

unsigned char DHTArray[5] = "\x00\x00\x00\x00\x00";
float LVar1, LVar2;
unsigned int Variable;
unsigned long DHTLastTimer;

void DHT_Initialize();
void FetchDHT(float*, float*);
#endif