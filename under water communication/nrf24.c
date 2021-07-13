#include <pic.h>
#include <string.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _SPI_H
#include "spi.h"
#endif

#include "nrf24.h"	

void NRF_WriteRegister(unsigned char Register, unsigned char *Data, unsigned int Length)
{
	CSN = 0;
	SPI_Write(W_REGISTER | (Register & REGISTER_MASK));
	SPI_SWrite(Data, Length);
	CSN = 1;
}

void NRF_ReadRegister(unsigned char Register, unsigned char *Data, unsigned int Length)
{
	CSN = 0;
	SPI_Write(R_REGISTER | (Register & REGISTER_MASK));
	SPI_SRW(Data, Length);
	CSN = 1;
}

void NRF_CommandRegister(unsigned char Register, unsigned char *Data, unsigned int Length)
{
	CSN = 0;
	SPI_Write(Register);
	SPI_SRW(Data, Length);
	CSN = 1;
}

void NRF_MaskS(unsigned char Register, unsigned char Data)
{
	unsigned char LocalVariable;
	NRF_ReadRegister(Register, &LocalVariable, 1);
	LocalVariable = LocalVariable | Data;
	NRF_WriteRegister(Register, &LocalVariable, 1);
}

void NRF_MaskC(unsigned char Register, unsigned char Data)
{
	unsigned char LocalVariable;
	NRF_ReadRegister(Register, &LocalVariable, 1);
	LocalVariable = LocalVariable & ~Data;
	NRF_WriteRegister(Register, &LocalVariable, 1);
}

void NRF_FlushTX()
{
	CSN = 0;
	SPI_Write(FLUSH_TX);
	CSN = 1;
}

void NRF_FlushRX()
{
	CSN = 0;
	SPI_Write(FLUSH_RX);
	CSN = 1;
}		

char NRF_ReadStatus()
{
	unsigned char LocalVariable;
	CSN = 0;
	LocalVariable = SPI_RW(STATUS);
	CSN = 1;
	return LocalVariable;
}

char NRF_ReadObserve()
{
	unsigned char LocalVariable;
	CSN = 0;
	LocalVariable = SPI_RW(OBSERVE_TX);
	CSN = 1;
	return LocalVariable;
}		

void NRF_PowerUp()
{
	unsigned char LocalVariable;
	NRF_ReadRegister(CONFIG_1, &LocalVariable, 1);
	
	if(!(LocalVariable & PWR_UP))
	{
		NRF_MaskS(CONFIG_1, PWR_UP);
		DelayMS(5);
	}
}

void NRF_PowerDown()
{
	CE = 0;
	NRF_MaskC(CONFIG_1, PWR_UP);
}

void NRF_SetAutoAck(char PipeNumber, const char Status)
{
	switch(PipeNumber)
	{
		case 0: 
		if(Status) NRF_MaskS(EN_AA, (1 << 0)); 
		else NRF_MaskC(EN_AA, (1 << 0));
		break;
	
		case 1:
		if(Status) NRF_MaskS(EN_AA, (1 << 1));
		else NRF_MaskC(EN_AA, (1 << 1));
		break;
		
		case 2:
		if(Status) NRF_MaskS(EN_AA, (1 << 2));
		else NRF_MaskC(EN_AA, (1 << 2));
		break;
		
		case 3:
		if(Status) NRF_MaskS(EN_AA, (1 << 3));
		else NRF_MaskC(EN_AA, (1 << 3));
		break;
		
		case 4:
		if(Status) NRF_MaskS(EN_AA, (1 << 4));
		else NRF_MaskC(EN_AA, (1 << 4));
		break;
		
		case 5:
		if(Status) NRF_MaskS(EN_AA, (1 << 5));
		else NRF_MaskC(EN_AA, (1 << 5));
		break;
		
		default:
		if(Status) NRF_MaskS(EN_AA, 0x3F);
		else NRF_MaskC(EN_AA, 0x3C); //Reset Value
		break;
	}
}

void NRF_DynamicPayload(char PipeNumber, const char Status)
{	
	switch(PipeNumber)
	{
		case 0: 
		if(Status) NRF_MaskS(DYNPD, (1 << 0)); 
		else NRF_MaskC(DYNPD, (1 << 0));
		break;
	
		case 1:
		if(Status) NRF_MaskS(DYNPD, (1 << 1));
		else NRF_MaskC(DYNPD, (1 << 1));
		break;
		
		case 2:
		if(Status) NRF_MaskS(DYNPD, (1 << 2));
		else NRF_MaskC(DYNPD, (1 << 2));
		break;
		
		case 3:
		if(Status) NRF_MaskS(DYNPD, (1 << 3));
		else NRF_MaskC(DYNPD, (1 << 3));
		break;
		
		case 4:
		if(Status) NRF_MaskS(DYNPD, (1 << 4));
		else NRF_MaskC(DYNPD, (1 << 4));
		break;
		
		case 5:
		if(Status) NRF_MaskS(DYNPD, (1 << 5));
		else NRF_MaskC(DYNPD, (1 << 5));
		break;
		
		default:
		if(Status) NRF_MaskS(DYNPD, 0x3F);
		else NRF_MaskC(DYNPD, 0x3F);
		break;
	}
}		

void NRF_EnableAckPayload(const char Status)
{
	if(Status) NRF_MaskS(FEATURE, EN_ACK_PAY);
	else NRF_MaskC(FEATURE, EN_ACK_PAY);
}

void NRF_MaskInterrupt(const char Status)
{
	if(Status) NRF_MaskC(CONFIG_1, ((MASK_RX_DR)|(MASK_TX_DS)|(1 << MASK_MAX_RT)));
	else NRF_MaskS(CONFIG_1, ((MASK_RX_DR)|(MASK_TX_DS)|(1 << MASK_MAX_RT)));
}

void NRF_ClearInterrupt()
{
	NRF_MaskS(STATUS, RX_DR);
	NRF_MaskS(STATUS, TX_DS);
	NRF_MaskS(STATUS, MAX_RT);
}	

void NRF_WriteAckPayload(char PipeNumber, unsigned char *Data, unsigned int Length)
{
	NRF_SetAutoAck(PipeNumber, True);
	NRF_EnableAckPayload(True);
	NRF_MaskS(EN_RXADDR, (1 << PipeNumber));
	NRF_CommandRegister(W_ACK_PAYLOAD | PipeNumber, Data, Length);
}

void NRF_Reuse()
{
	NRF_ClearInterrupt();
	CSN = 0;
	SPI_Write(REUSE_TX_PL);
	CSN = 1; CE = 0;
	__delay_us(10); CE = 1;
}

void NRF_ReTransmit(unsigned int Delay, unsigned char Count)
{
	unsigned char LocalVariable = Delay - 250;
	if(LocalVariable) LocalVariable /= 250;
	NRF_MaskS(SETUP_RETR, (((LocalVariable & 0x0F) << 4) | (Count & 0x0F)));
}

void NRF_TXRAddress(unsigned char *Address)
{
	unsigned char LocalVariable = 32;
	NRF_WriteRegister(TX_ADDR, Address, 5);
	NRF_WriteRegister(RX_ADDR_P0, Address, 5);
	NRF_WriteRegister(RX_PW_P0, &LocalVariable, 1);
}

void NRF_RXRAddress(char PipeNumber, unsigned char *Address)
{
	unsigned char LocalVariable = 32;
	NRF_WriteRegister(RX_ADDR_P0 | PipeNumber, Address, 5);
	NRF_WriteRegister(RX_PW_P0 | PipeNumber, &LocalVariable, 1);
}

char NRF_Transmit(unsigned char *Data, unsigned char *Length)
{
	NRF_PowerUp();
	NRF_ClearInterrupt();
	
	unsigned char FLength = Length[0];
	
	NRF_MaskS(EN_RXADDR, 0x00); //IS Receiver Power On?
	NRF_MaskC(CONFIG_1, PRIM_RX);
	
	NRF_FlushTX();
	NRF_CommandRegister(W_TX_PAYLOAD, Data, FLength);
	
	CE = 1; __delay_us(15); CE = 0; __delay_us(130); //Data Sended..?
	
	#if defined _TIMER_H
	StopTimer(); //Stop a while
	#endif
	
	unsigned int LocalTimer = 100;
	
	do
	{
		if(NRF_ReadStatus() & TX_DS) break;
		if(NRF_ReadStatus() & MAX_RT) return False;
		__delay_ms(1);
	}
	while(LocalTimer --> 0);
	
	#if defined _TIMER_H
	LastTicks += LocalTimer;
	RestartTimer(); //Start Again
	#endif
	
	unsigned char LocalVariable = NRF_ReadStatus();
	
	if(LocalTimer & RX_DR)
	{
		memset(Data, '\0', FLength);
		*Length = ((LocalVariable & ~0xF1) >> 1);
		NRF_CommandRegister(R_RX_PL_WID, &LocalVariable, 1);
		NRF_CommandRegister(R_RX_PAYLOAD, Data, LocalVariable);
		NRF_FlushRX();
	}
	
	NRF_PowerDown();
	NRF_FlushTX();
	
	return True;
}

char NRF_Receive(unsigned char *Data, unsigned char *PipeNumber)
{
	NRF_PowerUp();
//	NRF_ClearInterrupt(); //Instance Occur While We are Not Here..?
	
	unsigned char Pipe = PipeNumber[0];
	NRF_MaskS(EN_RXADDR, (1 << Pipe));
	NRF_MaskS(CONFIG_1, PRIM_RX);
	CE = 1; __delay_us(130); //Received..? Check..!
	
	unsigned char LocalVariable = NRF_ReadStatus();
	
	if(LocalVariable & RX_DR)
	{
		*PipeNumber = ((LocalVariable & ~0xF1) >> 1);
		NRF_CommandRegister(R_RX_PL_WID, &LocalVariable, 1);
		NRF_CommandRegister(R_RX_PAYLOAD, Data, LocalVariable);
		NRF_PowerDown();
		NRF_ClearInterrupt();
		NRF_FlushTX();
		NRF_FlushRX();
		return True;
	}
	return False;
}

void NRF_Initialize(unsigned char FreqChannel)
{
	SPI_Initialize();
	NRFDirection &=~ ((1 << CEPin)|(1 << CSNPin));
	CE = 0; CSN = 1; DelayMS(5);
	
	NRF_MaskS(RF_CH, (SET_RF_CH | FreqChannel));
	NRF_MaskS(RF_SETUP, (RF_1MBPS | RF_0DBM));
	
	NRF_MaskInterrupt(False);
	NRF_ClearInterrupt();
	NRF_PowerUp();
	NRF_MaskS(CONFIG_1, (EN_CRC | CRCO | PRIM_RX));
	
	NRF_FlushTX();
	NRF_FlushRX();
	
	NRF_MaskS(FEATURE, EN_DPL);
	NRF_ReTransmit(4000, 15); //Maximum..?
	NRF_MaskS(EN_RXADDR, 0x00);	
	NRF_SetAutoAck(6, True);
	NRF_DynamicPayload(6, True);
	NRF_EnableAckPayload(True);
}		 