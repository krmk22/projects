#ifndef _SERVO_H
#define _SERVO_H

#ifndef ServoConfig
#define ServoConfig
#define ServoPort P2
#define ServoPin 2
#endif

#ifndef ServoTimingConfig
#define ServoTimingConfig
#define MinimumAngle 0
#define MaximumAngle 180
#define MinimumPulse 544
#define MaximumPulse 2400
#endif

extern unsigned char ServoPosition;

void Servo_Initialize();
#endif