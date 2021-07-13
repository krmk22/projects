#ifndef _PWM_H
#define _PWM_H

#ifndef PulseWidth1AConfig
#define PulseWidth1AConfig
#define PulseWidth1APort PORTD
#define PulseWidth1ADirection DDRD
#define PulseWidth1APin PIND
#define PulseWidth1A PD6
#endif

#ifndef PulseWidth1BConfig
#define PulseWidth1BConfig
#define PulseWidth1BPort PORTD
#define PulseWidth1BDirection DDRD
#define PulseWidth1BPin PIND
#define PulseWidth1B PD5
#endif

#ifndef PulseWidth2AConfig
#define PulseWidth2AConfig
#define PulseWidth2APort PORTB
#define PulseWidth2ADirection DDRB 
#define PulseWidth2APin PINB
#define PulseWidth2A PB1
#endif

#ifndef PulseWidth2BConfig
#define PulseWidth2BConfig
#define PulseWidth2BPort PORTB
#define PulseWidth2BDirection DDRB
#define PulseWidth2BPin PINB
#define PulseWidth2B PB2
#endif

#ifndef PulseWidth3AConfig
#define PulseWidth3AConfig
#define PulseWidth3APort PORTB
#define PulseWidth3ADirection DDRB
#define PulseWidth3APin PINB
#define PulseWidth3A PB3
#endif

#ifndef PulseWidth3BConfig
#define PulseWidth3BConfig
#define PulseWidth3BPort PORTD
#define PulseWidth3BDirection DDRD
#define PulseWidth3BPin PIND
#define PulseWidth3B PD3
#endif

void PulseWidthModulation1A(unsigned char);
void PulseWidthModulation1B(unsigned char);
void PulseWidthModulation2A(unsigned char);
void PulseWidthModulation2B(unsigned char);
void PulseWidthModulation3A(unsigned char);
void PulseWidthModulation3B(unsigned char);

#endif
