/*
 * ADC.c
 *
 * Created: 24/03/2020 21:20:22
 *  Author: onion
 */ 

/*lib pour nom registre et format var*/
#include <avr/io.h>

/*header*/
#include "ADC.h"


/*
	Initialise l'ADC pour le capteur IR
*/
void ADC_init(){
	ADMUX=0x93;		/*	10 0(ADLAR) 1 0011 internal ref 2,56V + conversion sur PB3	*/
	ADCSRA=0xEE;	/*	1(EN) x(Start/ADSC) 1(ADATE) ?(flag) 1(ADIE/interupt en) 110 (64coups d'horloge pour mesurer)	*/

	ADCSRB=0;		/*	0?0--000 (free running mod)*/

}


/*
	Lance les mesures
*/
void ADC_start(){
	ADCSRA|=0x40;
}

/*
	Stop les mesures
*/
void ADC_stop(){
	ADCSRA&=~0x40;
}


/*
	Permet d'obtenir les mesures
*/
uint16_t ADC_getMesure(){
	/*voir page 137 datasheet*/
	uint8_t vL=ADCL;			/*ATTENTION A LORDRE DE LECTURE DE CES REGISTRE			*/
	uint8_t vH=ADCH;			/*JE SUIS RESTE BLOQUE UNE HEURE A CAUSE DE CETTE MERDE	*/
								/*BORDEL POURQUOI Y A UN ORDRE DE LECTURE ?????			*/

	return (vH<<8)+vL;			/*retourne la valeur*/
}

/*
	renvoie, à partir d'une valeur provenant de l'adc
	la tension en cV
*/
uint16_t traitement(uint16_t adc){
	return (uint16_t)((adc*256)/1024);
}