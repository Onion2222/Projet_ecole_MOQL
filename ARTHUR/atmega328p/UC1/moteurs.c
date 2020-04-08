/*
 * moteurs.c
 *
 * Created: 24/03/2020 00:31:30
 *  Author: onion
 * 
 */ 

/*lib pour le type uint8_t et pour le nom des registres*/
#include <avr/io.h>

/*header*/
#include "moteurs.h"

/*Definission des PINs***********/
/*moteur G PWM		:	16		*/
/*moteur G sense	:	13 PD7	*/
#define Gavance PORTD|=0x80
#define Grecule PORTD&=~0x80
/*moteur D PWM		:	15		*/
/*moteur D sense	:	14 PB0	*/
#define Davance PORTB|=0x01
#define Drecule PORTB&=~0x01


/*commutateur de rotation => 1 tounre 0 ne tourne pas*/
volatile uint8_t commutTourne=0;
/*commutateur de translation => 1 avance/recule 0 rien*/
volatile uint8_t commutAvance=0;


/*
	Initialise les pins definissant le sens du moteur
	Initialise la sortie PWM A et B
*/
void moteur_init(){
	PWM_init();							/*init PWM sur timer1 (PWM non connecté)*/
	DDRB|=0x01;							/*Init pin sens moteur D*/
	DDRD|=0x80;							/*Init pin sens moteur G*/				
	Gavance;							/*init sense moteur G sur avancer (PWM non connecté)*/
	Davance;							/*init sense moteur D sur avancer (PWM non connecté)*/
}

/*
	Fait avancer le robot tout droit à une certaine vitesse
	param: vitesse (10bits)
	vitesse du robot
	1: si avance/recule deja STOP
	2: sinon avancer
*/
void avancer_toggle(uint16_t vitesse){
	if(commutAvance){					/*Si déjà entrain d'avancer/reculer*/
		stop();							/*Stop*/
	}else{								/*sinon = si n'avance pas ou ne recule pas*/
		commutAvance=1;					/*modif variable état*/
		Gavance;						/*Moteur G avance*/
		Davance;						/*Moteur D avance*/
		PWM_setA(vitesse);				/*Application de la vitesse A*/
		PWM_setB(vitesse);				/*Application de la vitesse B*/
	}
}


/*
	Fait reculer le robot tout droit à une certaine vitesse
	param: vitesse (10bits)
	vitesse du robot
	1: si avance/recule deja STOP
	2: sinon reculer
*/
void reculer_toggle(uint16_t vitesse){
	if(commutAvance){					/*Si déjà entrain d'avancer/reculer*/
		stop();							/*Stop*/
	}else{								/*sinon = si n'avance pas ou ne recule pas*/
		commutAvance=1;					/*modif variable état*/
		Grecule;						/*Moteur G recule*/
		Drecule;						/*Moteur D recule*/
		PWM_setA(vitesse);				/*Application de la vitesse A*/
		PWM_setB(vitesse);				/*Application de la vitesse B*/
	}
}

/*
	Baisse le rapport cyclique à 0%
	!! N'arrete pas le PWM => un petit sursaut existe (contrefaçon du uc ?)
	utiliser PWM_stop();
*/
void stop(){
	commutTourne=0;						/*modif variable état*/
	commutAvance=0;						/*modif variable état*/
	Gavance;							/*Moteur G avance (état connu)*/
	Davance;							/*Moteur G avance (état connu)*/
	PWM_setA(0);						/*Application de la vitesse A*/
	PWM_setB(0);						/*Application de la vitesse B*/
}


/*
	Fonctionnement du toogle
	1: si tourne deja (dans un sens ou dans l'autre) STOP
	2: sinon tourner à gauche
	param: vitesse (10bits)
*/
void tournerGauche_toggle(uint16_t vitesse){
	if (commutTourne){					/*Si déjà entrain de tourner*/
		stop();							/*Stop*/
	}else{								/*sinon = si ne tourne pas*/
		commutTourne=1;					/*modif variable état*/
		Grecule;						/*Moteur G recule*/
		Davance;						/*Moteur D avance*/
		PWM_setA(vitesse);				/*Application de la vitesse A*/
		PWM_setB(vitesse);				/*Application de la vitesse B*/
	}
}


/*
	Fonctionnement du toogle
	1: si tourne deja (dans un sens ou dans l'autre) STOP
	2: sinon tourner à droite
	param: vitesse (10bits)
*/
void tournerDroite_toggle(uint16_t vitesse){
	if (commutTourne){					/*Si déjà entrain de tourner*/
		stop();							/*Stop*/
	}else{								/*sinon = si ne tourne pas*/
		commutTourne=1;					/*modif variable état*/
		Gavance;						/*Moteur G avance*/
		Drecule;						/*Moteur D recule*/
		PWM_setA(vitesse);				/*Application de la vitesse A*/
		PWM_setB(vitesse);				/*Application de la vitesse B*/
	}
}