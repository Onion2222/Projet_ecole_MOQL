#include <msp430.h>
#include <SPI.h>
#include <UART.h>
#include <board.h>
#include <Util.h>

#define CMDLEN  2


CHAR_8 retour[CMDLEN];
UINT_32 distance[CMDLEN];
BOOL balayageAuto = FALSE ;
BOOL afficherDistance = FALSE ;
extern BOOL marche_avant;

/**
 * main.c
 */

UINT_32 main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                   /* stop watchdog timer */

    if( (CALBC1_1MHZ==0xFF) || (CALDCO_1MHZ==0xFF) )
    {
        while(1);
    }
    else
    {
        /* Factory Set. */
        DCOCTL = 0;                               /* Select lowest DCOx and MODx settings */
        BCSCTL1 = CALBC1_1MHZ;                    /* Set DCO */
        DCOCTL = CALDCO_1MHZ;
    }

    init_board();
    init_uart();
    init_SPI();
    init_timer_com_spi();

    init_TimerMot();
    init_Robot();


    send_uart_l("=> MSP430 en ligne <=");
    send_uart_sdl();
    send_uart_sdl();

   __enable_interrupt();
    for(;;);

}


#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR()
{
    if (IFG2 & UCA0RXIFG)
    {

        while(!(IFG2 & UCA0RXIFG));
         UCHAR vC=UCA0RXBUF;
         UCHAR tC=UCB0RXBUF;

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
                break;
            case 'r':
                send_uart_l(">Marche arrière");send_uart_sdl();
                robotSens(1);
                break;
            case 'd':
                send_uart_l(">Tourner a droite");send_uart_sdl();
                robotSens(2);
                break;
            case 'g':
                send_uart_l(">Tourner a gauche");send_uart_sdl();
                robotSens(3);
                break;
            case 's':
                send_uart_l(">Stopper robot");send_uart_sdl();
                robotStop();
                break;
            case 'o':
                if (!balayageAuto){
                    send_uart_l(">Activation balayage capteur");send_uart_sdl();
                    balayageAuto = TRUE;
                }
                else{
                    send_uart_l(">Desactivation balayage capteur");send_uart_sdl();
                    balayageAuto = FALSE;
                }
                break;
            case 'f':

                if (!afficherDistance){
                    send_uart_l(">Activation affichage distance : ");send_uart_sdl();
                    afficherDistance = TRUE;
                }
                else{
                    send_uart_l(">Desactivation affichage distance : ");send_uart_sdl();
                    afficherDistance = FALSE;
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
            distance[0] = (INT_32)retour[0];


            if ( (afficherDistance) && (distance[0] != distance[1])   ){ /*Si demande d'affichage + distance différente à la derniere affichée */
                distance[1] = distance[0];
                dec_to_str(retour , distance[0] , 2);                    /*Conversion INT to string pour affichage */

                send_uart_l("Distance de l'obstacle :");
                send_uart_l(retour);
                send_uart_l("cm");
                send_uart_sdl();
            }

            if (distance[0] < 8 && marche_avant){
                    send_uart_l("OBSTACLE DETECTE");
                    send_uart_sdl();
                    robotStop();
            }
            retour[1] = 0x00;
        }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void ma_fnc_timer(void)
{
if (balayageAuto == 1){
    send_SPI(0x34);
    __delay_cycles(250);
}
send_SPI(0x50);
TA0CTL &= ~TAIFG; //RAZ TAIFG
}


