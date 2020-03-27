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
#include "SPI.h"


uint8_t angle=12;
uint8_t var_timer0_interupt=0;
uint8_t sens=0;
uint8_t obstacle=0;
uint16_t mesure_temp;


ISR(ADC_vect){

	if(ADC_getMesure()>=300){
		//si obstacle...
		obstacle=1;
	}else{
		//sinon...
		obstacle=0;
	}

}


ISR(TIMER0_OVF_vect){
	if(++var_timer0_interupt>=1)
	{
		
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



ISR (USI_OVF_vect){
	PORTB^=0x10; //PB3 test
	uint8_t reception=USIDR;

	switch (reception)
	{
	case 0x01:
		USIDR = 0x05;//ACK ping
		break;
	
	case 0x09: //rapport d'etat
		if(obstacle) USIDR=0x02;
		else USIDR=0x01;
		break;
	case 0x04: //capt first 8bits
		mesure_temp=ADC_getMesure();
		USIDR=(uint8_t)mesure_temp;
		break;
	case 0x05: //capt 2 bits left
		USIDR=(uint8_t)(mesure_temp>>8); //ATTENTION 10bit alors qu'envoie 8 !!!!
		break;
	}
	


	
	
	USISR = 1<<USIOIF;
	
	
	PORTB^=0x10; //PB3 test

}

int main(void)
{
	//PORTB=0xFF;//0000 1000
	DDRB=0x10;
	DDRB|=0x10; //PB4 test 0001 0000
	PORTB&=~0x10;

	
	PWM_timer1_init();
	PWM_timer1_start();
	
	timer0_init();
	
	ADC_init(); 
	
	SPI_USI_init();
	
	
	sei();
		
	for(;;){
	}
}

