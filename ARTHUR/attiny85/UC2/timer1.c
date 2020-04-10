/*
* timer1.c
*
* Created: 24/03/2020 19:34:30
*  Author: onion
*/

/*lib pour nom registre et format var*/
#include <avr/io.h>

/*header*/
#include "timer1.h"

#define MAX_PWM 16
#define MIN_PWM 4

/*
	initialise le PWM du timer 1
*/
void PWM_timer1_init(){
	TCCR1|=0x88;		/*	1(clear à OCR1C) 000(timerA) 1011(horloge predive 128 voir calcul) =>1000 1000	*/
	GTCCR|=0x40;		/*	? 1/0(ON/OFF) 01(set Hcomp pwm) 00(force value) 0 ? =>0x01 0000 pour debut en OFF	*/
	OCR1B=12;			/*	90 deg	*/
	OCR1C=156;			/*	max compteur pour 20ms	*/
}

/*
	Lance le signal PWM
*/
void PWM_timer1_start(){
	GTCCR|=(0x10); //CTL1 bit set 1
}

/*
	Arrete le signal PWM
*/
void PWM_timer1_stop(){
	GTCCR&=(0x10); //CTL1 bit set 0
}


/*
	permet de savoir si le PWM est actif ou pas
	retourne:
	1 si stoppé
	0 sinon
*/
uint8_t PWM_timer1_isStopped(){
	if((GTCCR&0x10)==0x10){			/* si stoppé*/
		return 0;					/* renvoie 0*/
	}
	else{							/*sinon*/
		return 1;					/* renvoie 1*/
	}
}

/*
	Modifie le rapport cyclique
	param:
	rapport cyclique en 8 bit
*/
void PWM_timer1_set(uint8_t ocr){
	if(ocr>MAX_PWM){				/*si ocr plus grand que max angle servo*/
		OCR1B=MAX_PWM;				/*application du max acceptable*/
		}else{						/*sinon*/
		if(ocr<MIN_PWM){			/*si ocr plus petit que min angle servo*/
			OCR1B=MIN_PWM;			/*application du min acceptable*/
			}else{					/*sinon (pas hors borne)*/
			OCR1B=ocr;				/*application ocr*/
		}
	}
}

/*
	retourne la valeur du pwm
*/
uint8_t PWM_timer1_get(){
	return OCR1B;
}

/*
	incremente rapport cylque
*/
void PWM_timer1_incr(){
	PWM_timer1_set(PWM_timer1_get()+1);
}

/*
	decremente rapport cylque
*/
void PWM_timer1_decr(){
	PWM_timer1_set(PWM_timer1_get()-1);
}