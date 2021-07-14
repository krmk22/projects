#ifndef _R307_H
#define _R307_H

extern char CollectFingerImage();
extern char ConvertImage2Char(unsigned char);

extern void FP_Initialize(unsigned long, char);
extern char FP_EnrollMent(unsigned char);
extern char FP_Identify(unsigned char*, unsigned char*);
extern char FP_Verify(unsigned char, unsigned char*);
extern char FP_Delete(unsigned char);
extern char FP_DeleteAll();
#endif