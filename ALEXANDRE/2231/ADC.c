
#include <msp430.h>
#include "ADC.h"
#include <Util.h>

void init_adc(void)
{
	  ADC10CTL0 = ADC10CTL1 = 0;
	  ADC10CTL0 =  SREF_0 + ADC10SHT_0  + ADC10ON;  ;
	  ADC10CTL1 =  ADC10DIV_0 + ADC10SSEL_2 +  SHS_0 + CONSEQ_0 ;

}


void ADC_Demarrer_conversion(UCHAR voie)
{
    ADC10CTL1 = (voie * 0x1000)+ ADC10DIV_0 + ADC10SSEL_2 +  SHS_0 + CONSEQ_0 ;
    ADC10CTL0 |= ENC + ADC10SC;
 }  


int ADC_Lire_resultat (void)
{
  	while (ADC10CTL1 & ADC10BUSY);
	ADC10CTL0 &= ~ENC;

    	return ADC10MEM;
}
