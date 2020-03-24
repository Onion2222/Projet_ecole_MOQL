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
#include "moteurs.h"
#include "SPI.h"

#define F_CPU 16000000UL
#define DEBUG_LED (1<<DDC5)


//variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)
volatile uint8_t var_clk=0;
volatile uint8_t var_t_acq_IR=0;
volatile uint8_t heartbeat=1;
volatile uint16_t vitesse=800;
volatile uint8_t override_obst=0;




ISR (TIMER0_COMPA_vect){
	if(++var_clk>=80){//execution toutes les secondes
		
		if(heartbeat==1){ //heartbeat, pour savoir si toutes les composantes du systemes sont OP
			PORTC^=DEBUG_LED; //clignotement de la LED de debug
			USART0_sendByte('*'); //envoie d'un heartbeat à l'UART
			
			//Heartbeat du salve SPI
			if(SPI_MasterEnvoieReception(0x01)==0x05){ //envoie de 0x01 (0x02 ACK)
				USART0_sendByte('^'); //envoie de '^' pour heartbeat de salve SPI
			}
			
			var_clk=0;
		}
		
	}
	if(++var_t_acq_IR>=8){ //toutes les 0,1sec
		//rapport d'état slave SPI
		if(SPI_MasterEnvoieReception(0x09)==0X02){ //si obstacle/erreur => voir tableau excel
			//get position capteur IR
			uint8_t angle =SPI_MasterEnvoieReception(0x0E); //voir tableau excel
			uint8_t mesure =SPI_MasterEnvoieReception(0x04); //voir tableau excel
			USART0_sendString("Obstacle position: {");
			USART0_sendByte(angle);
			USART0_sendByte(',');
			USART0_sendByte(mesure);
			USART0_sendString("}");
			if(!override_obst){
				stop();
			}
		}
		var_t_acq_IR=0;

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
		voir tableau excel pour la liste des commandes

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
		avancer_toggle(vitesse);
		break;

		case 'q': //gauche
		tournerGauche_toggle(vitesse);
		break;

		case 'd': //droite
		tournerDroite_toggle(vitesse);
		break;

		case 's': //recule
		reculer_toggle(vitesse);
		break;
		
		case '+':
		if(vitesse<1023) vitesse++; //pourquoi 1023 ? 3FF(10bits) voir init PWM
		if(PWM_getA()>0) PWM_setA(vitesse);
		if(PWM_getB()>0) PWM_setB(vitesse);
		break;
		
		case '-':
		if(vitesse>150) vitesse--;
		if(PWM_getA()>0) PWM_setA(vitesse);
		if(PWM_getB()>0) PWM_setB(vitesse);
		break;
		
		
		//commande envoie SPI slave
		case 'p':
		SPI_MasterEnvoie(0xB); //arret balayage
		SPI_MasterEnvoie(0xC);
		break;
		
		case 'm':
		SPI_MasterEnvoie(0xB); //arret balayage
		SPI_MasterEnvoie(0xD);
		break;
		
		case 'l':
		SPI_MasterEnvoie(0x6); //toggle balayage
		break;
		
		case 'o':
		override_obst=override_obst^1; //toggle override
		if(override_obst) USART0_sendString("Les obstacles ne seront plus traite");
		else USART0_sendString("Les obstacles seront maintenant traite");
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
	
	USART0_init(103); //init UART à 9600 Baud
	USART0_sendString("Demarrage du robot...");
	
	timer0_init(195); //init du timer
	
	moteur_init(); //init du PWM et des PINs moteurs
	
	SPI_init(); //init de la com SPI
	
	USART0_sendString("...OK!");
	
	
	for(;;);
}

