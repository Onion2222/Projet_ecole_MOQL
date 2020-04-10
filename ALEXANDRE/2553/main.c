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
            case 'a':
                robotSens(0);
                break;
            case 'r':
                robotSens(1);
                break;
            case 'd':
                robotSens(2);
                break;
            case 'g':
                robotSens(3);
                break;
            case 's':
                robotStop();
                break;
            case 'o':
                if (!balayageAuto){
                    balayageAuto = TRUE;
                }
                else{
                    balayageAuto = FALSE;
                }
                break;
            case 'f':
                if (!afficherDistance){
                    afficherDistance = TRUE;
                }
                else{
                    afficherDistance = FALSE;
                }
                break;
            default:
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
                send_uart_l(retour);
                __delay_cycles(100);

            }

            if (distance[0] < 8 && marche_avant){
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


