/*
* UC2.c
*
* Created: 20/03/2020 15:53:57
* Author : Onion
*/

/*
	!! Veuillez ne faire aucune optimisation sur les variables !!
*/


/*horloge de 1MHz*/

#include <avr/io.h>						/*lib pour les noms des registres format var*/
#include <avr/interrupt.h>				/*lib pour les interruptions*/

/*lib perso*/
#include "timer1.h"
#include "timer0.h"
#include "ADC.h"
#include "SPI.h"

#define SEUIL_CAPT_IR 300
#define MIN_ANGLE 9
#define MAX_ANGLE 16

uint8_t angle=12;								/*variable definissant l'angle du moteur*/
uint8_t var_timer0_interupt=0;					/*var pour gerer l'interrutpion du timer0 (freq d'execution)*/
uint8_t var_timer0_actif=1;						/*var pour gerer l'interrutpion du timer0 (1 execution/0 non) !! doit etre volatile sinon problèmes*/

uint8_t obstacle=0;								/*Variable definissant si obstacle ou pas 1=> obstacle*/
uint16_t mesure_temp;							/*Variable mesure temporaire ADC*/


/*
	Interruption executé à chaque mesure ADC
	!! doit etre fortement optimisée !!
*/
ISR(ADC_vect){

	if(ADC_getMesure()>=SEUIL_CAPT_IR){			/*Si obstacle*/
		obstacle=1;								/*Variable obstacle à 1*/
	}else{										/*sinon, variable obstalce à 0*/
		obstacle=0;
	}

}

/*
	Interruption sur timer0: execution periodique
*/
ISR(TIMER0_OVF_vect){
	
	/*modification angle du servo-moteur*/
	if((++var_timer0_interupt>=2)&var_timer0_actif){		/*ajustement du delay du timer (trop cours sur 8bits)*/
		
		if(angle==MIN_ANGLE){				/* si angle du servo au min */
			angle=MAX_ANGLE;				/* angle du servo au max */
		}
		else{								/* sinon, */
			if(angle==MAX_ANGLE){			/* si angle servo au max */
				angle=MIN_ANGLE;			/* angle du servo au min */
				}else{						/* sinon, (angle ni max ni min */
				angle=MIN_ANGLE;			/* angle du servo au min */
			}
		}
		PWM_timer1_set(angle);				/* application de l'angle via pwm*/
		var_timer0_interupt=0;				/*rest timer*/
	}
	

}


/*
	Interruption à chauqes reception sur l'USI = SPI
*/
ISR (USI_OVF_vect){

	uint8_t reception=USIDR;				/*recuperation donnée reçue*/


	/*
		Traitement commandes
		Voir commandes sur tableau excel
	*/
	switch (reception)
	{
		/*demande de ping*/
		case 0x01:							
		USIDR = 0x05;						/*envoie ACK*/
		break;
		
		/*demande rapport d'etat*/
		case 0x09:
		if(obstacle){						/*si obstacle*/
			USIDR=0x02;						/*envoie de NOK*/
		}
		else{								/*sinon*/
			USIDR=0x01;						/*envoie OK*/
		}
		break;
		
		/*Demande valeur capteur (1st 8 bit)*/
		case 0x04:
		mesure_temp=ADC_getMesure();		/*Capture de la valeur pour envoie des derniers bits à la prochaine commande*/
		USIDR=(uint8_t)mesure_temp;			/*traitement mesure et envoie*/
		break;
		/*Demande valeur capteur (2nd 8 bit)*/
		case 0x05:
		USIDR=(uint8_t)(mesure_temp>>8);	/*traitement mesure capturée à la commande precedente et envoie*/
		break;
		
		/*toggle balayage servo*/
		case 0x06:
		var_timer0_actif^=0x01;				/*desactivation/activation balayage interrupteur timer0*/
		break;
		
		/*lancement balayage*/
		case 0x0A:
		var_timer0_actif=0x01;				/*activation balayage interrupteur timer0*/	
		break;
		/*arret balayage*/
		case 0x0B:
		var_timer0_actif=0;					/*desactivation balayage interrupteur timer0*/
		break;
		
		/*incrementation de l'angle du servo*/
		case 0x0C:
			var_timer0_actif=0;				/*desactivation balayage interrupteur timer0*/
			PWM_timer1_incr();				/*incrementation du rapport cyclique*/
		break;
		/*decrementation de l'angle du servo*/
		case 0x0D: //angle servo --
			var_timer0_actif=0;				/*desactivation balayage interrupteur timer0*/
			PWM_timer1_decr();				/*decrementation du rapport cyclique*/
		break;
		
		/*commandes avec envoie de donnée*/
		default:
		/* ! application des masques ! */

		switch (reception&0xF0)
		{	
			/* envoie angle servo */
			case 0x10:
			var_timer0_actif=0;				/*desactivation balayage interrupteur timer0*/
			PWM_timer1_set(reception&0x0F); /*application de l'angle grace au masque*/
			break;
		}
		
		break;
	}
	

	USISR = 1<<USIOIF;						/*remise à 0 du flag -> pas auto ici*/


}

/*
	main essentiellement init
*/
int main(void)
{
	/*servo moteur en output*/
	DDRB=0x10;								/*PB4 OUTPUT (reste INPUT ou modif dans init)*/
	PORTB=0x10;								/*PB4 OUTPUT (reste INPUT ou modif dans init)*/

	
	PWM_timer1_init();						/*init pwm servo*/
	PWM_timer1_start();						/*lancement pwm servo*/
	
	timer0_init();							/*init interrupteur timer0*/
	
	ADC_init();								/*init ADC pour capt IR*/
	
	SPI_USI_init();							/*init USI en SPI*/
		
	
	sei();									/*activation interrupteur*/
	
	for(;;);								/*loop*/
}

