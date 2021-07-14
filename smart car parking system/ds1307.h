#ifndef _DS1307_H
#define _DS1307_H

#ifndef NumberConversion
#define NumberConversion
#define Hex2Dec(Data) (((Data / 16) * 10) + (Data % 16) * 1)
#define Dec2Hex(Data) (((Data / 10) * 16) + (Data % 10) * 1)
#endif

#ifndef DS1307Address
#define DS1307Address 0xD0
#endif

#ifndef RegisterAddress
#define RegisterAddress
#define RegSeconds 0x00
#define RegMinutes 0x01
#define RegHours 0x02
#define RegDay 0x03
#define RegDate 0x04
#define RegMonth 0x05
#define RegYear 0x06
#endif

void ds1307_Write(unsigned char, unsigned char);
unsigned char ds1307_Read(unsigned char);
void UpdateTime(unsigned char, unsigned char, unsigned char);
void UpdateDate(unsigned char, unsigned char, unsigned char, unsigned char);
void UpdateRTC(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
void FetchTime(unsigned char*, unsigned char*, unsigned char*);
void FetchDate(unsigned char*, unsigned char*, unsigned char*, unsigned char*);
void FetchRTC(unsigned char*, unsigned char*, unsigned char*, unsigned char*, unsigned char*, unsigned char*, unsigned char*);
#endif
