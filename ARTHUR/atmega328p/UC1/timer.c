/*
 * timer.c
 *
 * Created: 23/03/2020 18:04:10
 *  Author: onion
 */ 


/*lib pour nom registre*/
#include <avr/io.h>

/*header*/
#include "timer.h"


/*
	Cette fonction initialise le timer 0 (8bits)
	la valeur OCR0A permet de declencher l'interupteur TIMER0_COMPA
	Voir details des calculs dans le tableau Excel
*/
void timer0_init(uint8_t varOCR0A){
	/*init timer interrupt pour led debug, rapport d'etat*/
	/*timer 0 (8 bits ""malheureusement"")*/
	/*se referer au datasheet atmel*/
	TCCR0A=0x00; /*00 00 -- 00*/
	TCCR0B=0x05; /*0 0 -- 0 101 (/1024 prescaler)*/
	TCNT0=0; /*init*/
	OCR0A=varOCR0A; /*valeur à comparer avec TCNT0 pour générer inter*/
	TIMSK0=0x02; /*----- 010 gestion de l'interruption generé par timer0 (3 interuptions possibles => x1x = COMPA (OCR0A)*/
}