#ifndef _R308FP_H
#define _R308FP_H

#ifndef FPSensorPinConfig
#define FPSensorPinConfig
#define Red "VCC"
#define Black "GND"
#define Yellow "TXD"
#define White "RXD"
#define Green "TOUCH"
#define Blue "+V_TOUCH"
#endif

unsigned char FPArray[20], FPCount;

void FP_Routine();
void FP_Initialize();
char CollectFingerImage();
char ConvertImage2Char(unsigned char);
char FingerEnrollMent(unsigned char);
char FingerIdentify(unsigned char*, unsigned char*);
char FingerVerify(unsigned char, unsigned char*);
char FingerDelete(unsigned char);
char FingerDeleteAll();
char FingerTalkBack(const char*, unsigned int, unsigned int);
#endif