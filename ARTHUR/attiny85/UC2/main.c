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
uint8_t var_timer0_actif=1;

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
	if((++var_timer0_interupt>=2)&var_timer0_actif) //ajustement du delay du timer (trop cours sur 8bits)
	{
		
		if(angle==9){ //min 8
			angle=16; //max 17
		}
		else{
			if(angle==16){ //max 17
				angle=9; //min 8
				}else{
				angle=9; //min 8
			}
		}
		PWM_timer1_set(angle);
		var_timer0_interupt=0;
	}
	

}



ISR (USI_OVF_vect){

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
		case 0x05: //capt 2 bits left A EXECUTER APRES 0x04
		USIDR=(uint8_t)(mesure_temp>>8);
		break;
		
		case 0x06: //toggle balayage servo
		var_timer0_actif^=0x01;
		break;
		
		case 0x0A: //start balayage servo
		var_timer0_actif=0x01;
		break;
		case 0x0B: //stop balayage servo
		var_timer0_actif=0;
		break;
		
		case 0x0C: //angle servo ++
			var_timer0_actif=0;
			PWM_timer1_incr();
		break;
		case 0x0D: //angle servo --
			var_timer0_actif=0;
			PWM_timer1_decr();
		break;
		
		
		default: //pour donnée avec data
		// ! application des masques !!
		switch (reception&0xF0) //voir tableau excel
		{
			case 0x10: //envoie donnée servo (voir tableau excel)
			var_timer0_actif=0;
			
			PWM_timer1_set(reception&0x0F);
			
			break;
		}
		
		break;
	}
	

	USISR = 1<<USIOIF; //remise à 0 du flag -> pas auto ici


}

int main(void)
{
	DDRB=0x10; //PB4 OUTPUT (reste INPUT ou modif dans init)
	PORTB=0x10; //PB4 OUPUT (reste INPUT ou modif dans init)

	
	PWM_timer1_init(); //servo
	PWM_timer1_start(); //servo
	
	timer0_init();
	
	ADC_init();
	
	SPI_USI_init();
	
	
	sei();
	
	for(;;);
}

