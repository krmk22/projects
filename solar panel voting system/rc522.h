#ifndef _RC522_H
#define _RC522_H

#define RFID_OK 	0
#define RFID_NOTAG	1
#define RFID_ERR	2


extern unsigned char TagArray[16];

unsigned char RFID_Initialize(char, char, char, char, unsigned long);
void RegisterWrite(unsigned char, unsigned char);
void RegisterMaskS(unsigned char, unsigned char);
void RegisterMaskC(unsigned char, unsigned char);
unsigned char RegisterRead(unsigned char);

unsigned char CommandTag(unsigned char*, size_t, unsigned char*, unsigned char*);
unsigned char RequestTag(unsigned char, unsigned char*);
unsigned char AntiCollisionTag(unsigned char*);
void CalculateCRC(unsigned char*, size_t, unsigned char*);
unsigned char HaltTag();
unsigned char ReadRFID();
#endif