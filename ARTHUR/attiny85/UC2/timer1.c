/*
 * timer1.c
 *
 * Created: 24/03/2020 19:34:30
 *  Author: onion
 */ 

#include <avr/io.h>

#include "timer1.h"


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
	GTCCR&=(0x30); //CTL1 bit set 0
}

void PWM_timer1_set(uint8_t ocr){
	OCR1B=ocr;
}