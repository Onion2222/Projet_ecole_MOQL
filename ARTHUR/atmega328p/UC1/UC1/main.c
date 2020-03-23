/*
 * UC1.c
 *
 * Created: 20/03/2020 15:51:57
 * Author : Onion
 */ 
#include <avr/io.h>
#include<avr/interrupt.h> //lib pour utiliser interuptions

#define F_CPU 16000000UL
#define DEBUG_LED (1<<DDC5)

//variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)
volatile uint16_t var_clk=1;


ISR (TIMER0_COMPA_vect){
	
	if(++var_clk>=65500){
		PORTC^=DEBUG_LED; //clignotement de la LED de debug
		var_clk=0;
	}
	

}



/*
Cette fonction initialise le timer 0 (8bits)
la valeur OCR0A permet de declencher l'interupteur TIMER0_COMPA
Voir details des calculs dans le tableau Excel
*/
void timer0_init(){
	//init timer interrupt pour led debug
	//timer 0 (8 bits ""malheureusement"")
	//se referer au datasheet atmel
	TCCR0A=0x00; //00 00 -- 00
	TCCR0B=0x01; //0 0 -- 0 01 (/1024 prescaler)
	TCNT0=0; // (init) ?
	OCR0A=246; //valeur à comparer avec TCNT0 pour générer inter
	TIMSK0=0x02; //----- 010 gestion de l'interruption generé par timer0 (3 interuptions possibles => x1x = COMPA (OCR0A)
	//SE REFERER AUX CALCULS TABLEAU EXCEL
}






int main(void)
{
	//Init port C:
	DDRC = DEBUG_LED; //definition de la direction des pins du poirt C
	PORTC = DEBUG_LED; //extinction/allumage des ports C
	
	//init interrupt
	sei(); //activation interruptions global
	timer0_init(); //init du timer


    while (1) 
    {
		
		
		
    }
}

