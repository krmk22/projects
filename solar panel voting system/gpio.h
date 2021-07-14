#ifndef _GPIO_H
#define _GPIO_H

#define PORTSTATE &PORTA
#define TRISSTATE &TRISA
#define PINSTATE  &LATA

typedef enum
{
	PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
	PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
	PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7,
	PE0, PE1, PE2, PE3
}GPIO;

#define TRIS_OUTPUT(Pin) (*(TRISSTATE + (Pin >> 3)) &=~ (1 << (Pin & 0x07)))
#define TRIS_INPUT(Pin)	 (*(TRISSTATE + (Pin >> 3)) |=  (1 << (Pin & 0x07)))
#define TRIS_TOGGLE(Pin) (*(TRISSTATE + (Pin >> 3)) ^=  (1 << (Pin & 0x07)))

#define PORT_HIGH(Pin)	 (*(PINSTATE + (Pin >> 3)) |=  (1 << (Pin & 0x07)))
#define PORT_LOW(Pin)	 (*(PINSTATE + (Pin >> 3)) &=~ (1 << (Pin & 0x07)))
#define PORT_TOGGLE(Pin) (*(PINSTATE + (Pin >> 3)) ^=  (1 << (Pin & 0x07)))	

#define PORT_READ(Pin)   (*(PORTSTATE + (Pin >> 3)) & (1 << (Pin & 0x07)) ?1 :0)

#endif