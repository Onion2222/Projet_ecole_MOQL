/*
 * SPI.c
 *
 * Created: 25/03/2020 09:42:57
 *  Author: Onion
 */ 


#include <avr/io.h>


#include "SPI.h"


void SPI_USI_init(){
	//activation des ports
	//MOSI:	PB0 input
	//MISO:	PB1 output
	//SCK:	PB2 input? output selon datasheet -> marche avec: INPUT
	DDRB|=0x02; //0000 0010
	DDRB&=~0x05;//0000 0101
	
	USICR = ((1<<USIWM0)|(1<<USICS1));
	USICR |= (1<<USIOIE);
}