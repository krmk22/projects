#ifndef _SIM800L_H
#define _SIM800L_H

#define NO_GSM    0
#define NO_SIM   1
#define NO_NET   2
#define GSM_OK   3

#define ATTEND   1
#define DECLINE  0

#define ACK    1
#define NACK   0

#define DIALING  2
#define RINGING  3

extern unsigned char GSM_Found();
extern unsigned char GSM_Echo(unsigned char);
extern unsigned char GSM_SimStatus();
extern unsigned char GSM_PinStatus();
extern unsigned char GSM_Identifier(unsigned char);

extern unsigned char GSM_EnableGPRS(unsigned char);
extern unsigned char GSM_EnableMUX(unsigned char);
extern unsigned char GSM_StartGPRS(const char*, const char*);

extern unsigned char GSM_Initialize(unsigned long);
extern unsigned char GSM_SendMessage(const char*, const char*);
extern unsigned char GSM_ReceiveMessage(unsigned char*);
extern unsigned char GSM_Incoming(unsigned char*, unsigned char);
extern unsigned char GSM_Outgoing(const char*);
extern unsigned char GSM_OutgoingResult();
extern unsigned char GSM_CheckSignal();
#endif
