#ifndef _MATRIX_H
#define _MATRIX_H

extern void Matrix_Initialize(char, char, char, char, char, char, char, char);
extern unsigned char FetchKey(unsigned int*);
extern void Matrix(unsigned char, unsigned char*, unsigned char);
extern void RTC_Matrix(unsigned char*);
#endif