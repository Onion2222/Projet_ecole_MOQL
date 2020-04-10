#include <msp430.h>
#include <Util.h>

void init_uart(void){

    UCA0CTL1 |= UCSWRST;                      /* SOFTWARE RESET */

    P1SEL = BIT1 + BIT2 ;                     /* P1.1 = RXD, P1.2=TXD */
    P1SEL2 = BIT1 + BIT2 ;                    /* P1.1 = RXD, P1.2=TXD */

    UCA0CTL1 |= UCSSEL_2;                     /* Selection de l'horloge 1Mhz */
    UCA0BR0 = 9;                              /* 9600 baud > 1 000 000 / 115 000 = 8,7 */
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;                        /* USCI Second Stage Modulation Select 0 */
    UCA0CTL1 &= ~UCSWRST;                     /* Fin de reset - Activation */
    IE2 |= UCA0RXIE;                          /* Activation des interruption */
}

void send_uart(CHAR_8 vC){                    /* Envoi 1 caractère */

    while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF=vC;
}

void send_uart_sdl(void){                    /* Envoi 1 saut de ligne */
    while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF=0xD;

    while (!(IFG2&UCA0TXIFG));
    UCA0TXBUF=0xA;
}

void send_uart_l(const CHAR_8 *vStr){       /* Envoi 1 chaine de caractère */
    UINT_32 i=0;
    for(i=0;vStr[i]!=0x00;i++){             /* Tant qu'on est pas arriver au dernier char du tableau */
        while (!(IFG2&UCA0TXIFG));
        send_uart(vStr[i]);
    }
}
