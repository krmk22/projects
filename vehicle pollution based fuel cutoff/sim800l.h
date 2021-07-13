#ifndef _SIM800L_H
#define _SIM800L_H

//Receive Call Functions With Returning
#define ATTEND 1
#define DECLINE 0

void GSM_Initialize();
char GSM_SendMessage(const char*, const char*);
char GSM_ReceiveMessage(unsigned char*);
char GSM_Incoming(unsigned char*, const char);
char GSM_Outgoing(const char*);
char GSM_TalkBack(const char*, const char*, unsigned int);
#endif
