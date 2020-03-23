/*
 * UART.c
 *
 * Created: 23/03/2020 17:54:38
 *  Author: onion
 */ 

#include <avr/io.h>
#include "UART.h"




void USART0_init(uint16_t ubrr){
	//voir tableau excel pour calcul BAUD
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	UCSR0B = 0x98;//1 0 0 1 1 0? 0? 0?
	UCSR0C = 0x06;//00 00 0 1 1 0
}

void USART0_sendByte(unsigned char data){
	while (!( UCSR0A & (1<<UDRE0))); //attendre que le buffer se libere
	UDR0=data; //envoie
}

void USART0_sendString(char* data){
	uint8_t i=0;
	while ((i<255) & (data[i]!=0))
	{
		USART0_sendByte(data[i]); //envoie
		i++;
	}
	USART0_sendByte(13); //CR
	USART0_sendByte(10); //LF
}
