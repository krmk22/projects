#ifndef _KEYPAD_H
#define _KEYPAD_H

#ifndef FiveKeypadMod
#define FiveKeypadMod
#define SET 'S'
#define MOV 'M'
#define INC 'I'
#define DIC 'D'
#define ENT 'E'
#endif

void Keypad_Initialize(char, char, char, char, char);
char FetchKey(unsigned int*);
void Keypad(unsigned char, unsigned char*, unsigned char);
void RTC_Keypad(unsigned char*);
#endif
