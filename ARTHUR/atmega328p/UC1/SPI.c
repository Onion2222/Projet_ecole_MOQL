/*
* SPI.c
*
* Created: 24/03/2020 13:11:29
*  Author: Onion
*/

//nom registre
#include <avr/io.h>

//header
#include "SPI.h"

/*
Initalise la com SPI
*/
void SPI_init(){
	// MOSI et SCK en output le reste en input
	//MOSI:	PB3
	//SCK:	PB5
	//MISO:	PB4
	DDRB |= (1<<3)|(1<<5); //SCK MOSI output
	DDRB &= ~(1<<4); //MISO input

	//activation du SPI en master avec prescaler de /128
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
}

/*
Envoie 8 bits
Param: data => donnée à envoyer
*/
void SPI_MasterEnvoie(unsigned char data){
	SPDR = data; //ecriture de data dans buffer d'envoie
	while(!(SPSR & (1<<SPIF))); // attendre que la transmission se termine
}

/*
Envoie 8 bits puis attend la reception de 8 bits
Param: data => donnée à envoyer
Retourn: donnée reçue
*/
uint8_t SPI_MasterEnvoieReception(unsigned char data){
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	
	
	uint16_t i=0;
	for(i=500;i>1;i--); //attendre quelques temps
	
	
	SPDR = 0; //Envoie de 0 (pour envoyer les coups d'horologe)
	
	while(!(SPSR & (1 << SPIF))); //attndre fin com

	return SPDR;
}