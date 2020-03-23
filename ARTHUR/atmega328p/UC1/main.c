/*
 * UC1.c
 *
 * Created: 20/03/2020 15:51:57
 * Author : Onion
 */ 
#include <avr/io.h>
#include <avr/interrupt.h> //lib pour utiliser interuptions

#include "UART.h"
#include "timer.h"

#define F_CPU 16000000UL
#define DEBUG_LED (1<<DDC5)


//variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)
volatile uint16_t var_clk=1;
volatile uint8_t heartbeat=1;



ISR (TIMER0_COMPA_vect){	
	if(++var_clk>=80){//execution toutes les secondes
		if(heartbeat==1){
			PORTC^=DEBUG_LED; //clignotement de la LED de debug	
			USART0_sendByte('*'); //envoie d'un heartbeat à l'UART	
			var_clk=0;
		}
	}
}

ISR(USART_RX_vect){
	unsigned char ByteLu=UDR0;
	/*//repeteur
	USART0_send('>');
	USART0_sendByte(ByteLu);
	*/
	switch (ByteLu)
	{
	case 's':
		if(heartbeat==1){
			USART0_sendString("Arret heartbeat");
			heartbeat=0;
			PORTC&=!DEBUG_LED;
		}else{
			USART0_sendString("Lancement heartbeat");
			heartbeat=1;
		}
		break;		
	}
}




int main(void)
{
	//Init port C:
	DDRC = DEBUG_LED; //definition de la direction des pins du poirt C
	PORTC = DEBUG_LED; //extinction/allumage des ports C
	
	//init interrupt
	sei(); //activation interruptions global
	timer0_init(195); //init du timer
	USART0_init(103); //init UART à 9600 Baud
	
	USART0_sendString("Demarrage du robot");


    while (1) 
    {
		
		
		
    }
}

