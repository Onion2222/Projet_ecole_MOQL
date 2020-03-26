#include <msp430g2231.h>
#include <intrinsics.h>


#define MOTORS_FREQUENCY 33333 // motors period T=30 ms (0.0333 s)soit 30 Hz
#define MOTORS_DUTYCYCLE 1000 // motors duty cycle 50% soit 0.00125ms 1250 μs
#define STEP_ANGLE 72 // step for 1 deg PW[500-3000 μs]/180deg
#define PW_MIN 504 // 500/72=7 7*72=504
#define PW_MAX 2300 // 3000/72=41 41*72=2952

 void init_Board( void );
 void init_Timer( void );
 void commandServo(void);

/*
 * Variables Globales pour interruptions
 */
 unsigned int up = 0; // sens de variation
 unsigned int cmd = 0; // periode du signal de commande moteur
/**
 * main.c
 */

 unsigned char RXDta;



void commandServo(void){
    if( !up ) // Sens décroissant
             {
                 P1OUT &=~BIT6; // Eteindre la Led du port 1 ligne 6
                  if (cmd > (PW_MIN+STEP_ANGLE) ) // Si Période mini non encore atteinte
                  {
                      cmd -= STEP_ANGLE; // Décrémenter la période
                  }
                  else // Sinon
                  {
                      cmd = PW_MIN; // Ajuster la période
                      up = 1; // Changer le sens de boucle
                  }
                  P1OUT ^= BIT0; // Faire clignoter la Led a chaque itération
             }

             else // Sinon
             {
                 P1OUT &=~BIT0; // Eteindre la Led de P1.0
                 if(cmd < (PW_MAX-STEP_ANGLE) ) // Si Période inférieure au max
                 {
                     cmd += STEP_ANGLE; // Augmenter la période
                 }
                 else // Sinon
                 {
                     cmd = PW_MAX; // Ajuster la période
                     up = 0; // Inverser le sens de boucle
                 }
                 P1OUT ^= BIT6; // Faire clignoter la Led
              }
              TACCR1 = cmd; // Modifier la valeur de commptage Timer(Rapport Cyclique)
}


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


     P1SEL &= ~(BIT0 | BIT6); // Port 1, ligne 0 et 6 en fonction primaire

     P1DIR |= (BIT0 | BIT6 ); // P1.0 et P1.6 en sortie
     P1OUT &= ~(BIT0 | BIT6); // P1.0 et P1.6 à 0

     P1DIR &=~BIT3; // Port 1 ligne 3 en entrée
     P1REN |= BIT3; // Activation de la resistance de tirage
     P1OUT |= BIT3; // Resistance en Pull-Up
     P1IES &=~BIT3; // Detection de front montant
     P1IE |= BIT3; // Activation des interruptions sur P1.3

     P2SEL |= BIT6; // Port 2, ligne 6 en fonction secondaire
     P2DIR |= BIT6; // Port 2, ligne 6 en sortie
}
/**
 * Initialisation de l'USI pour la communication avec le MSP 2553
 */
void init_usi(){

    USICTL0 |= USISWRST;
    USICTL1 = 0;                                /* Reinit registre */

    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USIOE | USIGE| USILSB);   /* Configuration SPI SLave */
    USICTL0 &= ~(USIMST);

    USICTL1 |= USIIE;                           /*Activation  Interuptions*/
    USICTL1 &= ~(USICKPH | USII2C);

    USICKCTL = 0;                               /* Clock externe fournie par le MS2553 */

    USICKCTL &= ~(USICKPL | USISWCLK);  // Polarity - Input ClkLow

    USICNT = 0;
    USICNT &= ~(USI16B | USIIFGCC );            /* Only lower 8 bits used 14.2.3.3 p 401 slau144j */
    USISRL = 0x23;                              /* hash, just mean ready; USISRL Vs USIR by ~USI16B set to 0 */
    USICNT = 0x08;
}

/**
 * Intialisation du Timer pour le servo
 */

void init_Timer( void )
{
TACTL &= ~MC_0; // arret du timer
TACCR0 = MOTORS_FREQUENCY; // periode du signal PWM 2KHz
TACTL = (TASSEL_2 | MC_1 | ID_0 | TACLR); // select TimerA sourcecSMCLK, set mode to up-counting
TACCTL1 = 0 | OUTMOD_7; // select timer compare mode
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   /* stop watchdog timer */

    init_board();
    init_usi();
    init_Timer();

    cmd = MOTORS_DUTYCYCLE;
    up = 1;

    TACCR1 = cmd;

    //TACTL^=TAIE;

    while ((P1IN & BIT5)) ;
    USICTL0 &= ~USISWRST;
    //__bis_SR_register(LPM4_bits | GIE);             /* general interrupts enable & Low Power Mode */
    __bis_SR_register(LPM0_bits | GIE); // general interrupts enable & Lowwer Mode
}


#pragma vector=USI_VECTOR
__interrupt void universal_serial_interface(void)
{

    while( !(USICTL1 & USIIFG) );   /* Attente d'un caractère complet */
    RXDta = USISRL;

    if (RXDta == 0x31)
    {
        P1OUT |= BIT0; /* Allumage LED */
    }
    else if (RXDta == 0x30)
    {
        P1OUT &= ~BIT0; /* Extinction LED */
    }
    else if (RXDta == 0x32)
    {
        /* Clignotement LED */
        P1OUT&=~BIT0;
        TACTL^=TAIE;
    }
    else if (RXDta == 0x33)
    {
        commandServo(); // Commande servo
    }
    else if (RXDta == 0x34)
    {
        if(TACCR0<0x8000) TACCR0<<=1; /*diminuer la fréquence de clignotement LED*/
    }

    USISRL = RXDta;
    USICNT &= ~USI16B;  /* Reinitialisation du compteur */
    USICNT = 0x08;      /* Compteur sur 8Bits . Registre pret pour la nouvelle commande */

}

/* ************************************************************************* */
/* VECTEUR INTERRUPTION PORT 1 */
/* ************************************************************************* */
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
    if( P1IFG & BIT3) // interruption Entree/sortie Port 1 ligne 3
    { // si appui sur le bouton P1.3
     commandServo();
     P1IFG &= ~BIT3; // Acquiter l'interruption
    }
}
