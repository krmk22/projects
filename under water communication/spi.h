#ifndef _SPI_H
#define _SPI_H

#ifndef SPIPinConfig
#define SPIPinConfig
#define SPIPort PORTC
#define SPIDirection TRISC
#define SPIClock 4
#define SPIMISO 3
#define SPIMOSI 1
#endif

volatile bit SCK @ ((unsigned)&SPIPort*8) + SPIClock;
volatile bit MISO @ ((unsigned)&SPIPort*8) + SPIMISO;
volatile bit MOSI @ ((unsigned)&SPIPort*8) + SPIMOSI;
volatile bit DIN @ ((unsigned)&SPIPort*8) + SPIMISO;
volatile bit DOUT @ ((unsigned)&SPIPort*8) + SPIMOSI; 

void SPI_Initialize();
void SPI_Write(unsigned char);
unsigned char SPI_Read();
unsigned char SPI_RW(unsigned char);

void SPI_SWrite(unsigned char*, unsigned int);
void SPI_SRead(unsigned char*, unsigned int);
void SPI_SRW(unsigned char*, unsigned int);
#endif

