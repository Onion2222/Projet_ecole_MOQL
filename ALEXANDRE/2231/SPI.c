/*
 * LIBRAIRIE LIAISON SPI
 * MSP2553 EN SLAVE - 3 WIRES - 8 BITS - MSB FIRST
 * P1.5 CLOCK
 * P1.6 MISO
 * P1.7 MOSI
 */

#include <msp430.h>

/*
 * Initialisation de l'USI pour la communication avec le MSP 2553
 */
void init_usi(){

    USICTL0 |= USISWRST;
    USICTL1 = 0;                                /* Reinit registre */

    USICTL0 |= (USIPE7 | USIPE6 | USIPE5 | USIOE | USIGE| USILSB);   /* Configuration SPI SLave */
    USICTL0 &= ~(USIMST);

    USICTL1 |= USIIE;                           /*Activation  Interuptions*/
    USICTL1 &= ~(USICKPH | USII2C);

    USICKCTL = 0;                               /* Clock externe fournie par le MS2553 */

    USICKCTL &= ~(USICKPL | USISWCLK);  // Polarity - Input ClkLow

    USICNT = 0;
    USICNT &= ~(USI16B | USIIFGCC );            /* Only lower 8 bits used 14.2.3.3 p 401 slau144j */
    USISRL = 0x23;                              /* hash, just mean ready; USISRL Vs USIR by ~USI16B set to 0 */
    USICNT = 0x08;
}
