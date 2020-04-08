#include <msp430.h>
#include <SPI.h>
#include <UART.h>
#include <board.h>
#include <Util.h>

#define CMDLEN  2
unsigned char retour[CMDLEN];
unsigned int distance[2];
unsigned char balayageAuto = 0 ;//
unsigned char afficherDistance = 0 ;//
/**
 * main.c
 */
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
    init_SPI();
    init_timer_com_spi();

    init_TimerMot();
    init_Robot();


    send_uart_l("=> MSP430 en ligne <=");
    send_uart_sdl();
    send_uart_sdl();

   __enable_interrupt();
    //__bis_SR_register(LPM4_bits | GIE); // general interrupts enable & Low Power Mode

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

        switch(vC){
            case 'h':
                send_uart_l("==Bienvenue dans l'aide==");send_uart_sdl();
                send_uart_l("h - Aide");send_uart_sdl();
                send_uart_l("a - Avancer");send_uart_sdl();
                send_uart_l("r - Reculer ");send_uart_sdl();
                send_uart_l("d - Tourner droit");send_uart_sdl();
                send_uart_l("g - Tourner gauche");send_uart_sdl();
                send_uart_l("s - Stopper le robot");send_uart_sdl();
                send_uart_l("o - Commande servo moteur - detecteur obstacle");send_uart_sdl();
                send_uart_l("f - Afficher distance");send_uart_sdl();
                break;
            case 'a':
                send_uart_l(">Marche avant");send_uart_sdl();
                robotSens(0);
                robotMvt(1500,1500);
                break;
            case 'r':
                send_uart_l(">Marche arrière");send_uart_sdl();
                robotSens(1);
                robotMvt(1500,1500);
                break;
            case 'd':
                send_uart_l(">Tourner a droite");send_uart_sdl();
                robotSens(2);
                robotMvt(1200,1800);
                break;
            case 'g':
                send_uart_l(">Tourner a gauche");send_uart_sdl();
                robotSens(3);
                robotMvt(1800,1200);
                break;
            case 's':
                send_uart_l(">Stopper robot");send_uart_sdl();
                robotStop();
                break;
            case 'o':
                if (balayageAuto == 0){
                    send_uart_l(">Activation balayage capteur");send_uart_sdl();
                    balayageAuto = 1;
                }
                else{
                    send_uart_l(">Desactivation balayage capteur");send_uart_sdl();
                    balayageAuto = 0;
                }
                break;
            case 'f':

                if (afficherDistance == 0){
                    send_uart_l(">Activation affichage distance : ");send_uart_sdl();
                    afficherDistance = 1;
                }
                else{
                    send_uart_l(">Desactivation affichage distance : ");send_uart_sdl();
                    afficherDistance = 0;
                }
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
            retour[0] = UCB0RXBUF;
            distance[0] = (int)retour[0];


            if ( (afficherDistance == 1) && (distance[0] != distance[1])   ){
                distance[1] = distance[0];
                dec_to_str(retour , distance[0] , 2);

                send_uart_l("Distance de l'obstacle :");
                send_uart_l(retour);
                send_uart_l("cm");
                send_uart_sdl();
            }

            if (distance[0] < 8 ){
                    send_uart_l("OBSTACLE DETECTE");
                    send_uart_sdl();
                }
            retour[1] = 0x00;
        }
}

#pragma vector=TIMER0_A1_VECTOR //voir diaporama seance prputtyecedente
__interrupt void ma_fnc_timer(void)
{
if (balayageAuto == 1){
    send_SPI(0x34);
    __delay_cycles(250);
}
send_SPI(0x50);
TA0CTL &= ~TAIFG; //RAZ TAIFG
}


