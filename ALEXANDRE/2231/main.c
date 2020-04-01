#include <msp430g2231.h>
#include <intrinsics.h>
#include <SPI.h>
#include <board.h>
#include <ADC.h>
/*
 * Variables Globales pour interruptions
 */
 unsigned char RXDta;

/**
 * main.c
 */

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   /* stop watchdog timer */

    init_board();
    init_usi();
    init_timer();
    init_adc();


    while ((P1IN & BIT5)) ;
    USICTL0 &= ~USISWRST;
    __bis_SR_register(LPM0_bits | GIE); // general interrupts enable & Lowwer Mode
}

#pragma vector=USI_VECTOR
__interrupt void universal_serial_interface(void)
{

    while( !(USICTL1 & USIIFG) );   /* Attente d'un caractère complet */
    RXDta = USISRL;

    if (RXDta == 0x34)
    {
        commandServo();             /* Commande servo*/
    }

    USISRL = detectionObstacle();   /* retour = distance obstacle */
    USICNT &= ~USI16B;              /* Reinitialisation du compteur */
    USICNT = 0x08;                  /* Compteur sur 8Bits . Registre pret pour la nouvelle commande */

}
