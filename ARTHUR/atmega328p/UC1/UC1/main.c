/*
 * UC1.c
 *
 * Created: 20/03/2020 15:51:57
 * Author : Onion
 */ 
#define F_CPU 16000000UL
#define DEBUG_LED DDC5

ISR (TIMER0_COMPA_vect){
	PORTC^=DEBUG_LED;
}


#include <avr/io.h>
#include<avr/interrupt.h> //lib pour utiliser interuptions


int main(void)
{
	//Init port C:
	DDRC = DEBUG_LED; //definition de la direction des pins du poirt C
	PORTC = DEBUG_LED; //extinction/allumage des ports C
	
	//init interrupt
	sei(); //activation interruptions global
	
	//init timer interrupt pour led debug
	TCCR0A=0x00; //00 00 -- 00
	TCCR0B=0x01; //0 0 -- 0 01 (/1024 prescaler)
	TCNT0=0; // (init) ?
	OCR0A=1000; //valeur à comparer avec TCNT0 pour générer inter
	TIMSK0=0x02; //----- 010
	
	
	
    /* Replace with your application code */
    while (1) 
    {
		
		
		
    }
}

