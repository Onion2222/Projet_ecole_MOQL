/*
* timer1.c
*
* Created: 24/03/2020 19:34:30
*  Author: onion
*/

#include <avr/io.h>

#include "timer1.h"

#define MAX_PWM 16
#define MIN_PWM 4


void PWM_timer1_init(){
	TCCR1|=0x88;//1(clear à OCR1C) 000(timerA) 1011(horloge predive 128 voir calcul) =>1000 1000
	GTCCR|=0x40;//? 1(ON/OFF) 01(set Hcomp pwm) 00(force value) 0 ? =>0x01 0000
	OCR1B=12; //milieu
	OCR1C=156; //max compteur
}

void PWM_timer1_start(){
	GTCCR|=(0x10); //CTL1 bit set 1
}

void PWM_timer1_stop(){
	GTCCR&=(0x10); //CTL1 bit set 0
}


/*
1 si stoppé
0 sinon
*/
uint8_t PWM_timer1_isStopped(){
	if((GTCCR&0x10)==0x10) return 0; //start
	else return 1; //stop
}

void PWM_timer1_set(uint8_t ocr){
	if(ocr>MAX_PWM){
		OCR1B=MAX_PWM;
		}else{
		if(ocr<MIN_PWM){
			OCR1B=MIN_PWM;
			}else{
			OCR1B=ocr;
		}
	}
}
uint8_t PWM_timer1_get(){
	return OCR1B;
}

void PWM_timer1_incr(){
	PWM_timer1_set(PWM_timer1_get()+1);
}

void PWM_timer1_decr(){
	PWM_timer1_set(PWM_timer1_get()-1);
}