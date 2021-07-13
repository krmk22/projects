#ifndef _SIM800L_H
#define _SIM800L_H

#define ATTEND 1
#define DECLINE 0

#define NO_GSM 0
#define NO_SIM 1
#define NO_NET 2
#define GSM_OK 3

#define ACK 1
#define NACK 0

#define DIAL 0
#define RING 1
#define ATTD 2
#define DECL 3

char GSM_Initialize();
char GSM_SendMessage(const char*, const char*);
char GSM_ReceiveMessage(unsigned char*, const char);
char GSM_Incoming(unsigned char*, const char);
char GSM_Outgoing(const char*);
char GSM_TalkBack(const char*, const char*, unsigned int);
char GSM_OutgoingResult();
char GSM_CheckSignal();
#endif
