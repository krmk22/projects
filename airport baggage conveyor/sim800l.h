#ifndef _SIM800L_H
#define _SIM800L_H

#ifndef MC2GSM
#define MC2GSM SSerial
#endif

//GSM Possible Errors
#define NO_GSM 0
#define NO_SIM 1
#define NO_NET 2
#define GSM_OK 3

//Incoming Call Commands
#define ATTEND 1
#define DECLINE 0

//Message Acknowledges
#define ACK 1
#define NOACK 0

unsigned char GSMArray[60], GSMCount;

char GSM_Initialize();
char GSM_SendMessage(const char*, const char*);
char GSM_ReceiveMessage(unsigned char*, const char);
char GSM_Incoming(unsigned char*, const char);
char GSM_Outgoing(const char*);
char GSM_TalkBack(const char*, const char*, unsigned int);
char GSM_CheckSignal();
#endif
