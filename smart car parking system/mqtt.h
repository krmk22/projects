#ifndef _MQTT_H
#define _MQTT_H

#ifndef ServerConfig
#define ServerConfig
#define ServerName "industrial.api.ubidots.com"
#define ServerPort "1883"
#endif

#ifndef SecureConfig
#define SecureConfig
#define ClientName   "VankatOVenkat"
#define ClientID     "BBFF-l8L5Y2Dpcam8diQYNTwMFiOxxmm2m2"
#define Topic        "/v1.6/devices/"
#endif

#define DeviceLabel     "dlab"
#define VariableLabel1  "avaslot"
#define VariableLabel2  "regslot"
#define VariableLabel3  "slot1"
#define VariableLabel4  "slot2"
#define VariableLabel5  "slot3"
#define VariableLabel6  "slot4"
#define VariableLabel7  "register"

extern unsigned char MQTT_Connect();
extern unsigned char MQTT_Publish(const char*, unsigned int);
extern unsigned char MQTT_Subscribe(const char*, unsigned int);
extern unsigned char MQTT_Receive(const char*, unsigned int*);
extern unsigned char MQTT_Location(const char*, float, float);
#endif
