#ifndef _HEADER_H
#define _HEADER_H

#ifndef NumberSystem
#define NumberSystem
#define BIN 2
#define OCT 8
#define DEC 10
#define HEX 16
#endif

#ifndef ReturnConfig
#define ReturnConfig
#define True 1
#define False 0
#define Null '\0'
#endif

#ifndef SignSystem
#define SignSystem
#define Sign 1
#define NoSign 0
#endif

long MapValue(long, long, long, long, long);
int sstrlen(const char*, int);
char *smemchr(register const char*, int, int);
char *smemrchr(register const char*, int, int);
void *smemmem(const void*, int, const void*, int);
double power(double, int);
int dtoa(unsigned long, char*, int, const char);
int ftostra(float, int, int, char*, const char);
char SplitString(unsigned char*, unsigned char*, const char*, unsigned char);
#endif