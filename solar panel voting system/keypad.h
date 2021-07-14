#ifndef _KEYPAD_H
#define _KEYPAD_H

#define SET 'S'
#define MOV 'M'
#define ENT 'E'
#define INC 'I'
#define DIC 'D'

extern void Keypad_Initialize(char, char, char, char, char);
extern unsigned char FetchKey(unsigned int*);
extern void Keypad(unsigned char, unsigned char*, unsigned char);
extern void RTC_Keypad(unsigned char*);
#endif