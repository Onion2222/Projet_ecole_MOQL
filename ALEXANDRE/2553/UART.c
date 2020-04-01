#include <msp430.h>

void init_uart(){

    UCA0CTL1 |= UCSWRST;                       // SOFTWARE RESET

    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
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
    for(i=0;vStr[i]!=0x00;i++){
        send_uart(vStr[i]);
    }
}
