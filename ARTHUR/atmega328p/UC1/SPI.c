/*
* SPI.c
*
* Created: 24/03/2020 13:11:29
*  Author: Onion
*/

/*lib pour nom registre*/
#include <avr/io.h>

/*header*/
#include "SPI.h"



/*
	Initalise la com SPI
*/
void SPI_init(){
	/*
		MOSI et SCK en output le reste en input
		MOSI:	PB3
		SCK:	PB5
		MISO:	PB4
	*/
	DDRB |= (1<<3)|(1<<5);								/*SCK MOSI output*/
	DDRB &= ~(1<<4);									/*MISO input*/

	/*activation du SPI en master avec prescaler de /128*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
	/*
		SPIE:0 SPI interupt (inutile)
		SPE:1 SPI EN
		DORD:0 MSB first
		MSTR:1 M/S SELECT (pas utilis� mais doit etre declar�)
		CPOL:0  Rising edge
		CPHA:0 osef
		SPR[2:0]:011 presacaler 128 (max)
	*/
}

/*
	Envoie 8 bits
	Param: data => donn�e � envoyer
*/
void SPI_MasterEnvoie(unsigned char data){
	SPDR = data;										/*ecriture de data dans buffer d'envoie*/
	while(!(SPSR & (1<<SPIF)));							/*attendre que la transmission se termine*/
}

/*
	Envoie 8 bits puis attend la reception de 8 bits
	Param: data => donn�e � envoyer
	Retourn: donn�e re�ue
*/
uint8_t SPI_MasterEnvoieReception(unsigned char data){
	SPI_MasterEnvoie(data);								/*envoie de la donn�e*/
	
	/*Attendre traitement du slave*/
	uint16_t i=0;
	for(i=500;i>1;i--);
	
	SPDR = 0;											/*Envoie de 0 (pour envoyer les coups d'horologe)*/
	while(!(SPSR & (1 << SPIF)));						/*attendre fin com*/

	return SPDR;										/*Envoie reception*/
}