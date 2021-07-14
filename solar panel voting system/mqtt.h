#ifndef _MQTT_H
#define _MQTT_H

#ifndef ServerConfig
#define ServerConfig
#define ServerName "industrial.api.ubidots.com"
#define ServerPort "1883"
#endif

#ifndef SecureConfig
#define SecureConfig
#define ClientName 	"GuruCharan301"
#define ClientID 	"xxxxxxxxxxxxxxxxxxxxxxxx"
#define Topic		"/v1.6/devices/"
#endif

#define DeviceLabel 	"dlab"
#define VariableLabel1	"totalcount"
#define VariableLabel2	"party1"
#define VariableLabel3  "party2"
#define VariableLabel4  "result"

extern unsigned char MQTT_Connect();
extern unsigned char MQTT_Publish(const char*, unsigned int);
extern unsigned char MQTT_Subscribe(const char*, unsigned int);
extern unsigned char MQTT_Receive(const char*, unsigned int*);
extern unsigned char MQTT_Location(const char*, float, float);
#endif
