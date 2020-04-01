/*
 * LIBRAIRIE LIAISON SPI
 * MSP2553 EN MASTER - 3 WIRES - 8 BITS - MSB FIRST
 * P1.5 CLOCK
 * P1.6 MISO
 * P1.7 MOSI
 */

#include <msp430.h>

/*
 * Initialisation du SPI
 */
void init_SPI(){

    P1SEL|=(BIT5|BIT6|BIT7);
    P1SEL2|=(BIT5|BIT6|BIT7);

    UCB0CTL0 = 0;
    UCB0CTL1 = (0 + UCSWRST*1 );

    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

    UCB0CTL0 |= ( UCMST | UCMODE_0 | UCSYNC );
    UCB0CTL0 &= ~( UCCKPH | UCCKPL | UCMSB| UC7BIT );

    UCB0CTL1=(UCSSEL_2);
    UCB0BR0=0x0F;
    UCB0BR1=0x00; //division SMCLK par 1024
    UCB0CTL1&=~UCSWRST;

    IE2 |= UCB0RXIE;
    __delay_cycles(250);

}

/*
 * Permet d'envoyer une commmande vers le 2231.
 */

void send_SPI(unsigned char toSend){
    while ((UCB0STAT & UCBUSY));
    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF=toSend;

    P1OUT|=0x01; //clignotement LED
    __delay_cycles(250);
    P1OUT&=~0x01; //extinction LED
}
