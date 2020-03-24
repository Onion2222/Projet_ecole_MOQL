/*
 * moteurs.c
 *
 * Created: 24/03/2020 00:31:30
 *  Author: onion
 * ?? Hotel California ??
 */ 

//lib pour le type uint8_t
#include <avr/io.h>

//header
#include "moteurs.h"

//moteur G PWM		:	16
//moteur G sense	:	13 PD7
#define Gavance PORTD|=0x80
#define Grecule PORTD&=~0x80
//moteur D PWM		:	15
//moteur D sense	:	14 PB0
#define Davance PORTB|=0x01
#define Drecule PORTB&=~0x01

//commutateur de rotation => 1 tounre 0 ne tourne pas
volatile uint8_t commutTourne=0;
//commutateur de translation => 1 avance/recule 0 rien
volatile uint8_t commutAvance=0;


/*
  Initialise les pins definissant le sens du moteur
*/
void moteur_init(){
	PWM_init(); //init PWM sur timer1
	DDRB|=0x01;//
	DDRD|=0x80;//
	Gavance; //init sense
	Davance; //init sense
}

/*
 Fait avancer le robot tout droit à une certaine vitesse
 param: vitesse (10bits)
 vitesse du robot
 1: si avance/recule deja STOP
 2: sinon avancer
*/
void avancer_toggle(uint16_t vitesse){
	if(commutAvance){
		stop();
	}else{
		commutAvance=1;
		//roue G avancer
		Gavance;
		//roue D avancer
		Davance;
		PWM_setA(vitesse);
		PWM_setB(vitesse);
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
	if(commutAvance){
		stop();
	}else{
		commutAvance=1;
		//roue G reculer
		Grecule;
		//roue D reculer
		Drecule;
		PWM_setA(vitesse);
		PWM_setB(vitesse);
	}
}

/*
Baisse le rapport cyclique à 0%
!! N'arrete pas le PWM => utiliser PWM_stop();
*/
void stop(){
	commutTourne=0;
	commutAvance=0;
	//roue G osef
	Gavance;
	//roue D osef
	Davance;
	PWM_setA(0);
	PWM_setB(0);
}


/*
Fonctionnement du toogle
1: si tourne deja (dans un sens ou dans l'autre) STOP
2: sinon tourner à gauche
param: vitesse (10bits)
*/
void tournerGauche_toggle(uint16_t vitesse){
	if (commutTourne){ //si tourne deja
		stop();
	}else{
		commutTourne=1;
		//tourne
		//roue G reculer
		Grecule;
		//roue D avancer
		Davance;
		PWM_setA(vitesse);
		PWM_setB(vitesse);
	}
}


/*
Fonctionnement du toogle
1: si tourne deja (dans un sens ou dans l'autre) STOP
2: sinon tourner à droite
param: vitesse (10bits)
*/
void tournerDroite_toggle(uint16_t vitesse){
	if (commutTourne){ //si tourne deja
		stop();
	}else{
		commutTourne=1;
		//roue G avancer
		Gavance;
		//roue D reculer
		Drecule;
		PWM_setA(vitesse);
		PWM_setB(vitesse);
	}
}