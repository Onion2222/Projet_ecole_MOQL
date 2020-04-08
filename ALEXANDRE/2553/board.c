#include <msp430.h>
#include <Util.h>
 BOOL marche_avant = FALSE;
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
void init_TimerMot(void)
{
    TA1CTL = TASSEL_2 | MC_1;           /* source SMCLK pour TimerA , mode comptage Up */
    TA1CCTL1 |= OUTMOD_7;               /* activation mode de sortie n°7 sur TA1.1 */
    TA1CCTL2 |= OUTMOD_7;               /* activation mode de sortie n°7 sur TA1.2 */
    TA1CCR0 = 6500;                     /* determine la periode du signal */
}
/*
 * Permet d'activer les roues du robot
 */
void robotMvt(INT_32 dcA , INT_32 dcB)
{

    P2DIR |= (BIT2|BIT4);
    TA1CCR1 = dcA; /*Rapport cyclique de X% */
    TA1CCR2 = dcB;
    TA1CTL = TASSEL_2 | MC_1; /* Start du Timer */
}
/*
 * Stoppe les roues du robots
 */
void robotStop(void)
{
    marche_avant = TRUE;
    P2DIR &= ~(BIT2|BIT4);
    TA1CCR1 = 0;
    TA1CCR2 = 0;
    TA0CTL &= ~TAIFG; //RAZ TAIFG
}

void init_board(void){
    /*init P1 */
    P1SEL=0x00;
    P1SEL2=0x00;

    P1SEL = 0x00;        /* GPIO */
    P1DIR = 0x00;        /* IN   */

    /*Configuration LED */
    P1DIR |=  BIT0;
    P1OUT |= BIT0;
}

void init_Robot(void)
{
    P2DIR |= BIT2;                          /* P2.2 en sortie */
    P2SEL |= BIT2;                          /* selection fonction TA1.1 */
    P2SEL2 &= ~BIT2;                        /* selection fonction TA1.1 */
    P2DIR |= BIT4;                          /* P2.4 en sortie */
    P2SEL |= BIT4;                          /* selection fonction TA1.2 */
    P2SEL2 &= ~BIT4;                        /* selection fonction TA1.2 */
    P2DIR |= (BIT1|BIT5);                   /* Sens roue en sortie */
}

void robotSens(INT_32 sens)
{
    if(sens == 0) /* Avancer */
    {
        marche_avant = TRUE;
        robotMvt(2000,2000);
        P2OUT &= ~(BIT1);
        P2OUT |= BIT5;
    }
    if(sens == 1) /* Reculer */
    {
        marche_avant = FALSE;
        robotMvt(2000,2000);
        P2OUT |= BIT1;
        P2OUT &= ~BIT5;
    }
    if(sens == 2) /*Droite */
    {
        marche_avant = FALSE;
        robotMvt(2000,1000);
        P2OUT &= ~BIT1;
        P2OUT &= ~BIT5;
    }
    if(sens == 3) /* Gauche */
    {
        marche_avant = FALSE;
        robotMvt(1000,2000);
        P2OUT |= BIT1;
        P2OUT |= BIT5;
    }
}
