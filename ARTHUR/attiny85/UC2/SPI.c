/*
 * SPI.c
 *
 * Created: 25/03/2020 09:42:57
 *  Author: Onion
 */ 

/*lib pour nom registre et format var*/
#include <avr/io.h>

/*header*/
#include "SPI.h"


void SPI_USI_init(){
	/*
	activation des ports
		MOSI:	PB0 input
		MISO:	PB1 output
		SCK:	PB2 input? output selon datasheet -> marche avec: INPUT
	*/
	DDRB|=0x02;		/*0000 0010*/
	DDRB&=~0x05;	/*0000 0101*/
	
	USICR = ((1<<USIWM0)|(1<<USICS1));
	USICR |= (1<<USIOIE);
	/*
		USISIE:0 interrupt start com EN
		USIOIE:1 interrupt 8 coups d'hologe EN
		USIWM[1:0]:01 mode SPI (3 wire)
		USICS[0:1]:01 Software clock strobe (USICLK) => horloge maitre
		USICLK:0   ^^ ^^
		USITC:0 utilisation clock master
	*/
}