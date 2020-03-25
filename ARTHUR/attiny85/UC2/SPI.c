/*
 * SPI.c
 *
 * Created: 25/03/2020 09:42:57
 *  Author: Onion
 */ 


#include <avr/io.h>


#include "SPI.h"


void SPI_USI_init(){
	USICR=0x52;//0 1(COV interrupt) 01(3 wires) 001(master clock ?) 0
}