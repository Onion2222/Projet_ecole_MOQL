#include <msp430g2231.h>
#include <intrinsics.h>
#include <SPI.h>
#include <board.h>
#include <ADC.h>
#include <stdint.h>
/*
 * Variables Globales pour interruptions
 */
 unsigned char RXDta;
 unsigned int place, place2;
 unsigned char retour;

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

    retour = detectionObstacle();
    USISRL =  retour;               /* retour = distance obstacle */
    USICNT &= ~USI16B;              /* Reinitialisation du compteur bit à abit */
    USICNT = 0x08;                  /* Compteur sur 8Bits . Registre pret pour la nouvelle commande */

}
