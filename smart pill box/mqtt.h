#ifndef _MQTT_H
#define _MQTT_H

#ifndef ServerConfig
#define ServerConfig
#define ServerName "industrial.api.ubidots.com"
#define ServerPort "1883"
#endif

#ifndef SecureConfig
#define SecureConfig
#define ClientName   "AbsalMubeen"
#define ClientID     "BBFF-4rDFtDSf6lGtvq6Y85Yp641iTSSvNe"
#define Topic        "/v1.6/devices/"
#endif

#define DeviceLabel     "dlab"
#define VariableLabel1  "status1"
#define VariableLabel2  "status2"
#define VariableLabel3  "status3"
#define VariableLabel4  "status4"

extern unsigned char MQTT_Connect();
extern unsigned char MQTT_Publish(const char*, unsigned int);
extern unsigned char MQTT_Subscribe(const char*, unsigned int);
extern unsigned char MQTT_Receive(const char*, unsigned int*);
extern unsigned char MQTT_Location(const char*, float, float);
#endif
