/*
 * PWM.c
 *
 * Created: 23/03/2020 22:50:47
 *  Author: Onion
 */ 


//lib nom registres
#include <avr/io.h>

//header
#include "PWM.h"


//PWM sur PB1 et PB2
void PWM_init(){
	//PIN en output PB1 et PB2
	DDRB |= 0x06; //0000 0110
	PORTB &= ~(0x06); //0000 0110 mise à 0L des ports
	
	//TCCR1A
	// aa bb // ??
	//aa//bb: 00 -> pas de PWM
	//		: 11/10 -> PWM
	//
	TCCR1A=0x03; // ^^ ^^ -- 11(mode10bits) => ici 0000 0011=0x03 POUR fonctionnement 1111 0011=0xF3
	TCCR1B=0x09; // 00(osef) - 01(mode10bits) 001(prescal) => ici 0000 1001
	//tester 00 00 (mode) !
	
}

void PWM_setA(uint16_t OCR){
	OCR1AH=(uint8_t)(OCR>>8);
	OCR1AL=(uint8_t)OCR;
}

void PWM_setB(uint16_t OCR){
	OCR1BH=(uint8_t)(OCR>>8);
	OCR1BL=(uint8_t)OCR;
}


void PWM_start(){
	TCCR1A|=0xF0;
}
void PWM_stop(){
	TCCR1A&=0x0F;
}