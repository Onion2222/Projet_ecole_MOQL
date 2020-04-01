#include <msp430.h>

/*
 * INIT TIMER MOTEUR
 * INIT TIMER SPI
 * ACTIVATION MOTEUR
 * SENS MOTEUR
 * SECURISATION DES E/S
 */


/*
 * Timer permettant d'envoyant une commande à l'interface slave à interval régulier
 * afin d'obtenir un retour de variables provenant du 2231
 * Fréquence de 2HZ.
 */
void init_timer_com_spi(void){

    TA0CTL = 0|(TASSEL_2 | ID_3);
    TA0CTL |= MC_3;
    TA0CTL |= TAIE;
    TA0CCR0 = 62500;
}
/*
 * initialisation horloge robot
 */
void init_TimerMot()
{
    TA1CTL = TASSEL_2 | MC_1; // source SMCLK pour TimerA , mode comptage Up
    TA1CCTL1 |= OUTMOD_7; // activation mode de sortie n°7 sur TA1.1
    TA1CCTL2 |= OUTMOD_7; // activation mode de sortie n°7 sur TA1.2
    TA1CCR0 = 6500; // determine la periode du signal
}
/*
 * Permet d'activer les roues du robot
 */
void robotMvt()
{
    P2DIR |= (BIT2|BIT4);
    TA1CCR1 = 4000; /*Rapport cyclique de X% */
    TA1CCR2 = 4000;
    TA1CTL = TASSEL_2 | MC_1; // Start du Timer
}
/*
 * Stoppe les roues du robots
 */
void robotStop()
{
    P2DIR &= ~(BIT2|BIT4);
    TA1CCR1 = 0;
    TA1CCR2 = 0;
    TA0CTL &= ~TAIFG; //RAZ TAIFG
}

void init_Robot()
{
    P2DIR |= BIT2; // P2.2 en sortie
    P2SEL |= BIT2; // selection fonction TA1.1
    P2SEL2 &= ~BIT2; // selection fonction TA1.1
    P2DIR |= BIT4; // P2.4 en sortie
    P2SEL |= BIT4; // selection fonction TA1.2
    P2SEL2 &= ~BIT4; // selection fonction TA1.2
    P2DIR |= (BIT1|BIT5); // Sens roue en sortie
}

void robotSens(int sens)
{
    if(sens == 0) // Avancer
    {
        P2OUT &= ~(BIT1);
        P2OUT |= BIT5;
    }
    if(sens == 1) // Reculer
    {
        P2OUT |= BIT1;
        P2OUT &= ~BIT5;
    }
    if(sens == 2) // Droite
    {
        P2OUT &= ~BIT1;
        P2OUT &= ~BIT5;
    }
    if(sens == 3) // Gauche
    {
        P2OUT |= BIT1;
        P2OUT |= BIT5;
    }
}
