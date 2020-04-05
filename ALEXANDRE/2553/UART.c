#include <msp430.h>

void init_uart(){

    UCA0CTL1 |= UCSWRST;                      // SOFTWARE RESET

    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

    UCA0CTL1 |= UCSSEL_2;                     // Selection de l'horloge 1Mhz
    UCA0BR0 = 104;                            // 9600 baud > 1 000 000 / 9600 = 104
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;                        /* USCI Second Stage Modulation Select 0 */
    UCA0CTL1 &= ~UCSWRST;                     /* Fin de reset - Activation */
    IE2 |= UCA0RXIE;                          /* Activation des interruption */
}

void send_uart(char vC){

    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=vC; //envoie
}

void send_uart_sdl(){
    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=0xD; //envoie

    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=0xA;
}

void send_uart_l(const char *vStr){
    unsigned int i=0;
    for(i=0;vStr[i]!=0x00;i++){             /* Tant qu'on est pas arriver au dernier char du tableau */
        while (!(IFG2&UCA0TXIFG));
        send_uart(vStr[i]);
    }
}
