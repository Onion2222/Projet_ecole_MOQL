/*
 * LIBRAIRIE LIAISON SPI
 * MSP2553 EN MASTER - 3 WIRES - 8 BITS - MSB FIRST
 * P1.5 CLOCK
 * P1.6 MISO
 * P1.7 MOSI
 */

#include <msp430.h>
#include <Util.h>


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
 * Initialisation du SPI
 */
void init_SPI(){

    P1SEL|=(BIT5|BIT6|BIT7);                            /* Mode 3 Pin 1.5 SCLK 1.6 MISO 1.7 MOSI */
    P1SEL2|=(BIT5|BIT6|BIT7);

    UCB0CTL0 = 0;                                       /* RAZ Registre */
    UCB0CTL1 = (0 + UCSWRST*1 );

    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);                   /* Reinit Flag */

    UCB0CTL0 |= ( UCMST | UCMODE_0 | UCSYNC );          /* Master Mode , Mode 0 , Mode synchrone */
    UCB0CTL0 &= ~( UCCKPH | UCCKPL | UCMSB| UC7BIT );   /*Phase 0 , Polarité 0 , LSB First , 8 bits */

    UCB0CTL1=(UCSSEL_2);                                /* Reglage Horloge */
    UCB0BR0=0x0F;
    UCB0BR1=0x00;                                       /*division SMCLK par 1024*/

    UCB0CTL1&=~UCSWRST;                                 /* Activation */

    IE2 |= UCB0RXIE;                                    /* Activation Interruption */
    __delay_cycles(250);                                /* Attente init slave */

}

/*
 * Permet d'envoyer une commmande vers le 2231.
 */

void send_SPI(UCHAR toSend){
    while ((UCB0STAT & UCBUSY));                /* Attente etat msp */
    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF=toSend;

    P1OUT|=0x01;
    __delay_cycles(250);
    P1OUT&=~0x01;
}
