#ifndef _MQTT_H
#define _MQTT_H

#ifndef ServerConfig
#define ServerConfig
#define ServerName "industrial.api.ubidots.com"
#define ServerPort "1883"
#endif

#ifndef SecureConfig
#define SecureConfig
#define ClientName   "VirathVenkad37"
#define ClientID     "BBFF-ou550nBtFSQf1kqdB2MgBrBTX6AbHA"
#define Topic        "/v1.6/devices/"
#endif

#define DeviceLabel     "dlab"
#define VariableLabel1  "lamp"
#define VariableLabel2  "fan"
#define VariableLabel3  "door"
#define VariableLabel4  "switch"

extern unsigned char MQTT_Connect();
extern unsigned char MQTT_Publish(const char*, unsigned int);
extern unsigned char MQTT_Subscribe(const char*, unsigned int);
extern unsigned char MQTT_Receive(const char*, unsigned int*);
extern unsigned char MQTT_Location(const char*, float, float);
#endif
