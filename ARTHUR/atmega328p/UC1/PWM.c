/*
 * PWM.c
 *
 * Created: 23/03/2020 22:50:47
 *  Author: Onion
 * 
 */ 
//il faut regler le problèmes des sursauts à OCRnXw=0

//lib nom registres
#include <avr/io.h>

//header
#include "moteurs.h"



/*
init timer1 en PWM
*/
void PWM_init(){
	//PWM sur PB1 et PB2
	//PIN en output PB1 et PB2
	DDRB |= 0x06; //0000 0110
	PORTB &= ~(0x06); //0000 0110 mise à 0L des ports
	
	//TCCR1A
	// aa bb // ??
	//aa//bb: 00 -> pas de PWM
	//		: 11/10 -> PWM
	//
	TCCR1A=0x03; // ^^ ^^ -- 11(mode10bits) => ici 0000 0011=0x03 POUR fonctionnement 1010 0011=0xA3
	//COM1A[1:0]:00 pas de PWM sur A / :10 PWM sur A
	//COM1A[1:0]:00 pas de PWM sur B / :10 PWM sur B
	// -- reserved
	//WGM1[1:0]:11 PWM, phase correct, 10-bit(max)
	TCCR1B=0x09; // 00(osef) - 01(mode10bits) 001(prescal) => ici 0000 1001
	//ICNC1:0 noise osef
	//ICES1:0 capture en
	//- reserved
	//WGM1[3:2]:01 PWM, phase correct, 10-bit(max)
	//CS1[2:0]:001 pas de prescaler (peut etre à modifier selon moteur)
	
}


/*
Modifie rapport cyclique sortie A
Param: nouvelle valeur entre 0 et 1024
*/
void PWM_setA(uint16_t OCR){
	if(OCR==0) PWM_stop(); //etrangement, il reste un petit "sursaut" de PWM à 0
	else PWM_start();
	OCR1AH=(uint8_t)(OCR>>8);
	OCR1AL=(uint8_t)OCR;
}

/*
Modifie rapport cyclique sortie B
Param: nouvelle valeur entre 0 et 1024
*/
void PWM_setB(uint16_t OCR){
	if(OCR==0) PWM_stop(); //etrangement, il reste un petit "sursaut" de PWM à 0
	else PWM_start();
	OCR1BH=(uint8_t)(OCR>>8);
	OCR1BL=(uint8_t)OCR;
}

/*
get rapport cyclique A
retourn rapport cyclique effectif
*/
uint16_t PWM_getA(){
	return OCR1AL + (OCR1AH<<8);
}

/*
get rapport cyclique B
retourn rapport cyclique effectif
*/
uint16_t PWM_getB(){
	return OCR1BL + (OCR1BH<<8);
}

/*
Active PWM
*/
void PWM_start(){
	TCCR1A|=0xA0;
}

/*
Desactive PWM
*/
void PWM_stop(){
	TCCR1A&=0x0F;
}
