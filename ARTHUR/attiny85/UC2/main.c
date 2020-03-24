/*
* UC2.c
*
* Created: 20/03/2020 15:53:57
* Author : Onion
*/

//horloge de 1MHz

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer1.h"
#include "timer0.h"
#include "ADC.h"


uint8_t angle=12;
uint8_t var_timer0_interupt=0;
uint8_t sens=0;


ISR(ADC_vect){

	if(ADC_getMesure()>=400){
		//si obstacle...
	}else{
		sinon...
	}

}


ISR(TIMER0_OVF_vect){
	if(++var_timer0_interupt>=1)
	{
		/*switch (sens)
		{
			case 1:
			if(angle++>=16) sens=sens^1;
			break;
			case 0:
			if(angle--<=7) sens=sens^1;
			break;
		}*/
		if(angle==5){ //min 3
			angle=13; //max 17
		}
		else{
			if(angle==13){ //max 17
				angle=5; //min 3
			}else{
				angle=5; //min 3
			}
		}
		PWM_timer1_set(angle);
		var_timer0_interupt=0;
	}
	

}


int main(void)
{
	//PORTB=0xFF;//0000 1000
	DDRB=0x10;
	
	
	
	PWM_timer1_init();
	PWM_timer1_start();
	
	timer0_init();
	ADC_init();
	
	
	sei();

	for(;;){
		
	}
}

