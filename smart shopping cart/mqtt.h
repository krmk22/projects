#ifndef _MQTT_H
#define _MQTT_H

#ifndef ServerConfig
#define ServerConfig
#define ServerName "industrial.api.ubidots.com"
#define ServerPort "1883"
#endif

#ifndef SecureConfig
#define SecureConfig
#define ClientName   ""
#define ClientID     ""
#define Topic        "/v1.6/devices/"
#endif

#define DeviceLabel     "dlab"
#define VariableLabel1  "price"
#define VariableLabel2  "pay"
#define VariableLabel3  ""
#define VariableLabel4  ""
#define VariableLabel5  ""

extern unsigned char MQTT_Connect();
extern unsigned char MQTT_Publish(const char*, unsigned int);
extern unsigned char MQTT_Subscribe(const char*, unsigned int);
extern unsigned char MQTT_Receive(const char*, unsigned int*);
extern unsigned char MQTT_Location(const char*, float, float);
#endif
