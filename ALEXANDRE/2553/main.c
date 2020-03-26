#include <msp430.h>

#define CMDLEN  10
unsigned char cmd[CMDLEN];      // tableau de caracteres lie a la commande user
/**
 * main.c
 */
void init_uart(){
    //P1.1 et P1.2 en mode uart
    P1SEL |= (BIT1 | BIT2);
    P1SEL2 |= (BIT1 | BIT2);

    UCA0CTL1 |= UCSWRST;                        // SOFTWARE RESET

    UCA0CTL0=0;
    UCA0CTL1=(UCSSEL_2); //horloge interne
    UCA0BR0=104; //9600 baud
    UCA0BR1=0; //9600 baud
    UCA0MCTL = 10;

    UCA0CTL0 &= ~(UCPEN  | UCMSB | UCDORM);
    UCA0CTL0 &= ~(UC7BIT | UCSPB  | UCMODE_3 | UCSYNC); // dta:8 stop:1 usci_mode3uartmode

    UCA0CTL1 &= ~UCSWRST; //reset flag

    //interrupt
    IE2 |= UCA0RXIE;
}

void send_uart(char vC){

    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=vC; //envoie
    //IFG2&=~UCA0TXIFG; //
}

void send_uart_sdl(){
    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=0xD; //envoie
    //IFG2&=~UCA0TXIFG; //
    while (!(IFG2&UCA0TXIFG)); //busy ?
    UCA0TXBUF=0xA;
}

void send_uart_l(const char *vStr){
    unsigned int i=0;
    for(i=0;vStr[i]!=0x00;i++){
        send_uart(vStr[i]);
    }
}


/*void Init_clignotement(){
    BCSCTL1=CALBC1_1MHZ;//frequence d’horloge 1MHz
    DCOCTL=CALDCO_1MHZ;//"

    TA0CTL=0|(TASSEL_2|ID_3);//source SMCLK , /8 de predivision
    TA0CTL|=MC_3;//comptage en mode up/down

    TA0CCR0=0x8000; //0 à 65535
}*/



void init_SPI(){


    P1SEL|=(BIT5|BIT6|BIT7);
    P1SEL2|=(BIT5|BIT6|BIT7);


   // UCB0CTL0=0x00;
    //UCB0CTL1=UCSWRST;


    UCB0CTL0 = 0;
    UCB0CTL1 = (0 + UCSWRST*1 );

    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);

    UCB0CTL0 |= ( UCMST | UCMODE_0 | UCSYNC );
    UCB0CTL0 &= ~( UCCKPH | UCCKPL | UCMSB| UC7BIT );

    //UCB0CTL0=(UCCKPH|UCMST|UCMSB|UCSYNC|UCMODE_0);

    //UCB0STAT=UCLISTEN; //ecoute SPI activé

    UCB0CTL1=(UCSSEL_2);
    UCB0BR0=0x0F;
    UCB0BR1=0x00; //division SMCLK par 1024
    //USCI_B1=BIT3; //interrupt anabled for TX
    UCB0CTL1&=~UCSWRST;

    IE2 |= UCB0RXIE;
    __delay_cycles(250);


}

void send_SPI(unsigned char toSend){
    while ((UCB0STAT & UCBUSY));
    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF=toSend;

    //IFG2&=~UCB0TXIFG;


    P1OUT|=0x01; //clignotement LED
    __delay_cycles(250);
    P1OUT&=~0x01; //extinction LED
}



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    if( (CALBC1_1MHZ==0xFF) || (CALDCO_1MHZ==0xFF) )
    {
        while(1);
    }
    else
    {
        // Factory Set.
        DCOCTL = 0;                               // Select lowest DCOx and MODx settings
        BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
        DCOCTL = CALDCO_1MHZ;
    }


    //init P1
    P1SEL=0x00;
    P1SEL2=0x00;

    P1SEL = 0x00;        /* GPIO */
    P1DIR = 0x00;        /* IN   */

    /*Configuration LED */
    P1DIR |=  BIT0;
    P1OUT |= BIT0;
    init_uart();
    //Init_clignotement();

    send_uart_l("=> MSP430 en ligne <=");
    send_uart_sdl();
    send_uart_sdl();

    //test spi loopback
    init_SPI();

    __bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode

    for(;;);

}



#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR()
{
    if (IFG2 & UCA0RXIFG)
    {

        while(!(IFG2 & UCA0RXIFG));
        unsigned char vC=UCA0RXBUF;
        unsigned char tC=UCB0RXBUF;
        //unsigned char vC = UCA0RXBUF;
        send_uart_l("Caractere recu: ");
        send_uart(vC);
        send_uart_sdl();
        switch(vC){
            case 'h':
                send_uart_l("==Bienvenue dans l'aide==");send_uart_sdl();
                send_uart_l("h - Aide");send_uart_sdl();
                send_uart_l("e - eteindre la LED");send_uart_sdl();
                send_uart_l("a - allumer la LED");send_uart_sdl();
                send_uart_l("c - faire clignoter la LED");send_uart_sdl();
                send_uart_l("+ - Augmenter le frequence de clignotement");send_uart_sdl();
                send_uart_l("- - Diminuer la frequence de clignotement");send_uart_sdl();
                break;
            case 'e':
                send_uart_l(">Extinction de la LED");send_uart_sdl();
                send_SPI(0x30);
                break;
            case 'a':
                send_uart_l(">Allumage de la LED");send_uart_sdl();
                send_SPI(0x31);
                break;
            case 'c':
                send_uart_l(">Clignotement de la LED");send_uart_sdl();
                send_SPI(0x32);
                break;
            case '+':
                send_uart_l(">Fréquence augmentée");send_uart_sdl();
                //if(TA0CCR0>35)TA0CCR0-=TA0CCR0/10;
                send_SPI(0x33);
                break;
            case '-':
                send_uart_l(">Fréquence diminuée");send_uart_sdl();
                //if(TA0CCR0<58900)TA0CCR0+=TA0CCR0/10;
                send_SPI(0x34);
                break;
            default:
                send_uart_l("commande inconnue");send_uart_sdl();
                break;
            }

        }
        else if (IFG2 & UCB0RXIFG)
        {
            while( (UCB0STAT & UCBUSY) && !(UCB0STAT & UCOE) );
            while(!(IFG2 & UCB0RXIFG));
            cmd[0] = UCB0RXBUF;
            send_uart_l("retour recu: ");
            send_uart(cmd[0]);
            send_uart_sdl();
            cmd[1] = 0x00;

        }
}


