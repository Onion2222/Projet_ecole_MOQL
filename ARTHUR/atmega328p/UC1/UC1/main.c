/*
 * UC1.c
 *
 * Created: 20/03/2020 15:51:57
 * Author : Onion
 */ 
#include <avr/io.h>
#include <avr/interrupt.h> //lib pour utiliser interuptions

#define F_CPU 16000000UL
#define DEBUG_LED (1<<DDC5)
#define ubrr 103

//variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)
volatile uint16_t var_clk=1;


ISR (TIMER0_COMPA_vect){
	
	if(++var_clk>=65500){
		PORTC^=DEBUG_LED; //clignotement de la LED de debug
		var_clk=0;
		USART0_send(65);
		USART0_send(66);
		USART0_send(67);
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

void USART0_init(){
	//voir tableau excel pour calcul BAUD
	UBRR0H = (ubrr>>8);
	UBRR0L = (ubrr);
	UCSR0B = 0x98;//1 0 0 1 1 0? 0? 0?
	UCSR0C = 0x06;//00 00 0 1 1 0
}

void USART0_send(unsigned int data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0))));
	/* Put data into buffer, sends the data */
	UDR0 = data;
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

