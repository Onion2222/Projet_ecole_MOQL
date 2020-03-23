/*
 * UC1.c
 *
 * Created: 20/03/2020 15:51:57
 * Author : Onion
 */ 

//lib atmel general
#include <avr/io.h>
#include <avr/interrupt.h> //lib pour utiliser interuptions

//lib perso
#include "UART.h"
#include "timer.h"
#include "PWM.h"

#define F_CPU 16000000UL
#define DEBUG_LED (1<<DDC5)


//variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)
volatile uint16_t var_clk=1;
volatile uint8_t heartbeat=1;

//UNIQUEMENT POUR TEST
volatile uint16_t pwmA=600;
volatile uint16_t pwmB=600;



ISR (TIMER0_COMPA_vect){	
	if(++var_clk>=80){//execution toutes les secondes
		if(heartbeat==1){
			PORTC^=DEBUG_LED; //clignotement de la LED de debug	
			USART0_sendByte('*'); //envoie d'un heartbeat à l'UART	
			var_clk=0;
		}
	}
}


/*
Interruption reception UART
*/
ISR(USART_RX_vect){
	
	//petit truc con: UDR0 sert de buffer reception ET envoie 
	unsigned char ByteLu=UDR0; //enregistrement de la valeur lu
	
	
	/*
	//repeteur
	USART0_send('>');
	USART0_sendByte(ByteLu);
	*/

	//analyseur syntaxique
	/*Commandes:
		
		SYSTEME:
		a -> commutation heartbeat (cligno LED + envoie '*' UART) ON/OFF
		e -> etat du robot (ISP Capt_IR niveau batterie) => UART est forcement bon :D
		
		MOTEUR:
		z -> avance
		q -> tourne gauche
		d -> tourne droite
		s -> recule

	*/
	switch (ByteLu)
	{

		case 'a': //commutation heartbeat
			if(heartbeat==1){
				USART0_sendString("Arret heartbeat");
				heartbeat=0;
				PORTC &= ~DEBUG_LED;
			}else{
				USART0_sendString("Lancement heartbeat");
				heartbeat=1;
			}
			break;		
		

		case 'e':
			//diagnostique
			USART0_sendString("Debut diagnostique");
			//SPI

			//Capteur IR

			//niveau batterie
		
		break;

		case 'z': //avance
			PWM_setA(++pwmA); //test SEULEMENT
		break;

		case 'q': //gauche
			PWM_setA(--pwmA); //test SEULEMENT
		
		break;

		case 'd': //droite
			PWM_setB(++pwmB); //test SEULEMENT
		
		break;

		case 's': //recule
			PWM_setB(--pwmB); //test SEULEMENT
		break; 

		default:
			USART0_sendByte('>');
			USART0_sendByte(ByteLu);
			USART0_sendString(" => Commande inconnue !");
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
	PWM_init(); //init PWM sur timer1
	
	USART0_sendString("Demarrage du robot");

	
	PWM_start(); //test SEULEMENT

    for(;;);
}

