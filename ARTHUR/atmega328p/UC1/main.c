/*
* UC1.c
*
* Created: 20/03/2020 15:51:57
* Author : Onion
*/

/*lib atmel general*/
#include <avr/io.h>										/*lib pour noms des registres, etc*/
#include <avr/interrupt.h>								/*lib pour utiliser interuptions*/

/*lib perso*/
#include "UART.h"
#include "timer.h"
#include "moteurs.h"
#include "SPI.h"


#define F_CPU 16000000UL								/*Definission de l'horloge cadencé à 16MHz*/
#define DEBUG_LED (1<<DDC5)								/*Definission du port de la LED de debug*/

#define VITESSE_MAX_PWM 1023
#define VITESSE_MIN_PWM 150


/*variable permettant de baisser la frequence de l'interruption TIMER0_COMPA (registre 8 bit trop court)*/
volatile uint8_t var_clk=0;								/*Pour l'heartbeat*/
volatile uint8_t var_t_acq_IR=0;						/*Pour le rapport d'état au slave*/

/*Differentes varibales definissant l'état du robot*/
volatile uint8_t heartbeat=1;							/*Variable definissant l'activation (1) ou la desactivation (0) de l'heartbeat*/
volatile uint16_t vitesse=800;							/*Variable definissant la vitesse des moteurs*/
volatile uint8_t override_obst=0;						/*Variable definissant l'activation (1) ou la desactivation (0) de l'arret automatique*/



/*
	Interruption du timer0 à la comparaison de A
	S'execute toute les 0.01248s pour une valeur OCCRA=195
	Voir tableau Excel pour plus d'infos
*/
ISR (TIMER0_COMPA_vect){
	
	/*execution toutes les secondes (voir tableau excel)*/
	if(++var_clk>=80){
		
		if(heartbeat==1){								/*Si heartbeat activé*/
			/*heartbeat, pour savoir si toutes les composantes du systemes sont OP*/
			PORTC^=DEBUG_LED;							/*clignotement de la LED de debug*/
			USART0_sendByte('*');						/*envoie d'un heartbeat à l'UART*/
		}
		
		var_clk=0;										/*Reset de la var assistant le timer*/
		
	}
	
	/*toutes les 0,4sec (voir tableau excel)*/
	if(++var_t_acq_IR>=32){
		
		/*demande du rapport d'état au slave SPI*/
		if(SPI_MasterEnvoieReception(0x09)==0x02){		/*demande rapport avec reponse => obstacle ? (obstacle=0x02)*/
			
			/*Recuperation état slave (ange/mesure)*/
			uint8_t angle =SPI_MasterEnvoieReception(0x0E);		/*voir tableau excel*/
			uint16_t mesure =SPI_MasterEnvoieReception(0x04);	/*voir tableau excel*/
			mesure +=(SPI_MasterEnvoieReception(0x05)<<8);		/*voir tableau excel*/
			
			/*Communication des données à l'utilisateur via l'UART*/
			USART0_sendString("Obstacle position: {");
			USART0_sendByte(angle);
			USART0_sendByte(',');
			USART0_sendByte(mesure);
			USART0_sendString("}");
			
			/*Arret du robot (ou pas)*/
			if(!override_obst){							/*si arret auto activé*/
				stop();
			}
		}
		var_t_acq_IR=0; /*Reset de la var assistant le timer*/
	}
}


/*
	Interruption reception UART
	S'execute à chaques reception d'un char sur l'UART
	Traite la donnée reçue
*/
ISR(USART_RX_vect){
	
	/*petit truc con: UDR0 sert de buffer reception ET envoie*/
	unsigned char ByteLu=UDR0;							/*Acquisition de la donné reçue*/
	
	
	/*
	//repeteur
	USART0_send('>');
	USART0_sendByte(ByteLu);
	*/

	/******analyseur syntaxique*****
		Commandes:
		voir tableau excel pour la liste des commandes
	*/
	switch (ByteLu)
	{
		/*commutation heartbeat*/
		case 'a': 
		if(heartbeat==1){								/*si heartbeat activé*/
			USART0_sendString("Arret heartbeat");		/*com utilisateur*/
			heartbeat=0;								/*desactivation heartbeat*/
			PORTC &= ~DEBUG_LED;						/*Exctinctiond de la led de debug*/
		}else{											/*si heartbeat desactivé*/
			USART0_sendString("Lancement heartbeat");	/*com utilisateur*/
			heartbeat=1;								/*activation heartbeat*/
		}
		break;
		
		/*diagnostique de la com SPI*/
		case 'e':
		USART0_sendString("Debut diagnostique");		/*com utilisateur*/
		/*Demande ACK du slave*/
		if(SPI_MasterEnvoieReception(0x01)==0x05){		/*Envoie/reception ACK voir tableau excel => si réponse SPI*/
			USART0_sendString("SPI: OK");				/*com utilisateur SPI OK*/
		}else{											/*si pas de reponse SPI*/
			USART0_sendString("SPI: NOK");				/*com utilisateur SPI nOK*/
		}
		break;
		
		/*commande robot avancer*/
		case 'z':
		avancer_toggle(vitesse);						/*active/deactive avancer*/
		break;
		
		/*commande robot tourner à gauche*/
		case 'q':
		tournerGauche_toggle(vitesse);					/*active/deactive tourner à gauche*/
		break;
		
		/*commande robot tourner à droite*/
		case 'd':
		tournerDroite_toggle(vitesse);					/*active/deactive tourner à droite*/
		break;
		
		/*commande robot reculer*/
		case 's':
		reculer_toggle(vitesse);						/*active/deactive reculer*/
		break;
		
		/*commande augmentation vitesse*/
		case '+':
		if(vitesse<VITESSE_MAX_PWM){					/*si PWM pas au max*/
			vitesse++;									/*Augmentation de la variable vitesse*/
		}
		if(PWM_getA()>0){								/*Si PWM A pas désactivé*/
			PWM_setA(vitesse);							/*Application de la vitesse*/
		}
		if(PWM_getB()>0){								/*Si PWM B pas désactivé*/
			PWM_setB(vitesse);							/*Application de la vitesse*/
		}
		break;
		
		/*commande diminution vitesse*/
		case '-':
		if(vitesse>VITESSE_MIN_PWM){					/*si PWM pas au min*/
			vitesse--;									/*Diminution de la variable vitesse*/
		}
		if(PWM_getA()>0){								/*Si PWM A pas désactivé*/
			PWM_setA(vitesse);							/*Application de la vitesse*/
		}
		if(PWM_getB()>0){								/*Si PWM B pas désactivé*/
			PWM_setB(vitesse);							/*Application de la vitesse*/
		}
		break;
		
		/**commande envoie SPI slave**/
		
		/*Demande augmentation angle servo-moteur*/
		case 'p':
		SPI_MasterEnvoie(0x0C);							/*demande augmentation angle servo (arret automatique du servo)*/
		break;
		
		/*Demande diminution angle servo-moteur*/
		case 'm':
		SPI_MasterEnvoie(0x0D);							/*demande diminution angle servo (arret automatique du servo)*/
		break;
		
		/*Demande fin balayage du servo-moteur*/
		case 'l':
		SPI_MasterEnvoie(0x06);							/*Commande desactivation balayage*/
		break;
		
		
		/*Fin arret auto si obstacle*/
		case 'o':
		override_obst=override_obst^1;					/*toggle override*/
		if(override_obst){
			USART0_sendString("Les obstacles ne seront plus traite");		/*com utilisateur*/
		}
		else{
			USART0_sendString("Les obstacles seront maintenant traite");	/*com utilisateur*/
		}
		break;

		/*Commande inconnue*/
		default:
		USART0_sendByte('>');							/*com utilisateur*/
		USART0_sendByte(ByteLu);						/*retour char reçu*/
		USART0_sendString(" => Commande inconnue !");	/*com utilisateur*/
		break;

	}
}



/*main, ne concerne que les init...*/
int main(void)
{
	/*Init port C:
		DEBUG_LED: output
	*/
	DDRC = DEBUG_LED;									/*definition de la direction des pins du poirt C*/
	PORTC = DEBUG_LED;									/*extinction/allumage des ports C*/
	
	
	USART0_init(103);									/*init UART à 9600 Baud*/
	USART0_sendString("Demarrage du robot...");			/*com utilisateur*/
	
	timer0_init(195);									/*init du timer pour interrupt timer0*/
	
	moteur_init();										/*init du PWM et des PINs moteurs*/
	
	SPI_init();											/*init de la com SPI*/
	
	USART0_sendString("...OK!");						/*com utilisateur*/
	
	sei();												/*activation interruptions global*/
	
	for(;;);											/*boucle infinie, rien à faire*/
}

