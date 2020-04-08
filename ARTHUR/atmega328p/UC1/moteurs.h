/*
 * moteurs.h
 *
 * Created: 23/03/2020 22:51:02
 *  Author: Onion
 * 
 */ 



/*PWM.c*/
void PWM_init();
void PWM_setA(uint16_t OCR);
void PWM_setB(uint16_t OCR);
void PWM_start();
void PWM_stop();
uint16_t PWM_getA();
uint16_t PWM_getB();

/*moteurs.c*/
void moteur_init();
void avancer_toggle(uint16_t vitesse);
void reculer_toggle(uint16_t vitesse);
void stop();
void tournerGauche_toggle(uint16_t vitesse);
void tournerDroite_toggle(uint16_t vitesse);