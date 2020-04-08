/*
 * timer0.c
 *
 * Created: 24/03/2020 19:37:23
 *  Author: onion
 */ 

/*lib pour nom registre et format var*/
#include <avr/io.h>

/*header*/
#include "timer0.h"

void timer0_init(){
	GTCCR|=0x00;	/*safe						*/
	TCCR0A=0x00;	/*0000--00					*/
	TCCR0B=0x05;	/*00--0 101 (prescaler)		*/
	TIMSK|=0x02;	/*-XX00X1-					*/
}