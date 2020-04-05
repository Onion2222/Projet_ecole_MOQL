#include <msp430.h>
#include <board.h>
#include <ADC.h>

#define MOTORS_FREQUENCY 20000 // motors period T=20 ms soit 50 Hz
#define MOTORS_DUTYCYCLE 1460 // motors duty cycle 7,8% . Position initiale du servo
#define STEP_ANGLE 178 // pas pour 10°
#define PW_MIN 600 // rapport cyclique min 3%
#define PW_MAX 2200 // max 11%

 unsigned int up = 0; // sens de variation
 unsigned int cmd = 0; // periode du signal de commande moteur
 unsigned int  result_adc =0 ;
 unsigned int  distance = 0;


 /**
  * Intialisation du Timer pour le servo
  */
void init_timer( void )
{
TACTL &= ~MC_0; // arret du timer
TACCR0 = MOTORS_FREQUENCY; // periode du signal PWM 2KHz
TACTL = (TASSEL_2 | MC_1 | ID_0 | TACLR); // select TimerA sourcecSMCLK, set mode to up-counting
TACCTL1 = 0 | OUTMOD_7; // select timer compare mode
}

/*Initialisation et securisation E/S
 *
 */
void init_board(){

    if(CALBC1_1MHZ==0xFF || CALDCO_1MHZ==0xFF)
    {
        __bis_SR_register(LPM4_bits);
    }
    else
    {
        /* Factory Set.*/
        DCOCTL = 0;
        BCSCTL1 = CALBC1_1MHZ;
        DCOCTL = (0 | CALDCO_1MHZ);
   }

    //--- Securisation des entrees/sorties
    P1SEL = 0x00; // GPIO
    P2SEL = 0x00; // GPIO
    P1DIR = 0x00; // IN
    P2DIR = 0xFF; // IN
    //---

     P1SEL &= ~(BIT0 |BIT3| BIT6); // Port 1, ligne 0 et 6 en fonction primaire

     P1DIR |= BIT6; // P1.0 et P1.6 en sortie
     P1OUT &= ~BIT6; // P1.0 et P1.6 à 0

     P1DIR &=~(BIT3 | BIT0); // Port 1 ligne 3 en entrée
     P1REN |= (BIT3 | BIT0); // Activation de la resistance de tirage
     P1OUT |= (BIT3 | BIT0); // Resistance en Pull-Up
     P1IES &=~BIT3; // Detection de front montant
     P1IE |= BIT3; // Activation des interruptions sur P1.3

     P2SEL |= BIT6; // Port 2, ligne 6 en fonction secondaire
     P2DIR |= BIT6; // Port 2, ligne 6 en sortie

     cmd = MOTORS_DUTYCYCLE;
     up = 1;

     TACCR1 = cmd;

}

int  detectionObstacle(){
    ADC_Demarrer_conversion(0);
    result_adc = ADC_Lire_resultat();
    distance = (1017 - result_adc) / 23;
    return distance;
}

void commandServo(void){
    if( !up )                                     /* Si sens décroissant*/
             {
                  if (cmd > (PW_MIN+STEP_ANGLE) ) /* Si position min non atteinte*/
                  {
                      cmd -= STEP_ANGLE;          /* -10° */
                  }
                  else
                  {
                      cmd = PW_MIN;               /* position = min */
                      up = 1;                     /* Changer le sens du moteur */
                  }
             }

             else                                 /* Si sens croissant */
             {
                 if(cmd < (PW_MAX-STEP_ANGLE) )   /* Si position max non atteinte  */
                 {
                     cmd += STEP_ANGLE;           /* +10° */
                 }
                 else
                 {
                     cmd = PW_MAX;                 /* position = max */
                     up = 0;                       /* Changer le sens du moteur */
                 }
              }
              TACCR1 = cmd;
}
