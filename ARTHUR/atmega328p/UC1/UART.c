/*
 * UART.c
 *
 * Created: 23/03/2020 17:54:38
 *  Author: onion
 */ 

/*lib nom registres*/
#include <avr/io.h>

/*header*/
#include "UART.h"


/*
	Initialise la communication UART
	Voir datasheet pour plus d'informations
	param: ubrr
	ubrr donne le baud de la communication, se referrer au tableau excel pour plus d'informations sur son calcul
*/
void USART0_init(uint16_t ubrr){
	/*voir tableau excel pour calcul BAUD*/
	UBRR0H = (uint8_t)(ubrr>>8);			/*def baud 1/2*/
	UBRR0L = (uint8_t)ubrr;					/*def baud 2/2*/
	UCSR0B = 0x98;	/*	1(interrupt en RX) 0(interrupt en TX) 0(interrupt data reg empty) 1(RX en) 1(TX en) 0(8 bits) 0(9e bit) 0(9e bit)	*/
	UCSR0C = 0x06;	/*	00(Asynchrone) 00(parity disabled) 0(1 stop bit) 11(8bit) 0(clock polarity: falling edge)							*/
}


/*
	Envoie un octet (byte) de donn�e
	param: data
	data est la donn�e � envoyer 
*/
void USART0_sendByte(unsigned char data){
	while (!( UCSR0A & (1<<UDRE0)));		/*attendre que le buffer se libere*/
	UDR0=data;								/*envoie de data en l'inscivant dans le buffer d'envoi*/
}


/*
	Envoie un string et CR+LF (saut de ligne)
	param: data
	String � envoyer
	(utilise USART0_senByte(...))
*/
void USART0_sendString(char* data){
	
	/*Traitement du string*/
	uint8_t i=0;
	while ((i<255) & (data[i]!=0))			/*tant que le traitement du string n'est pas finie (limite � 255 pour pas faire le fou)*/
	{
		USART0_sendByte(data[i]);			/*envoie via fonction USART0_sendByte*/
		i++;
	}
	/*com sdl*/
	USART0_sendByte(13); /*CR*/
	USART0_sendByte(10); /*LF*/
}


