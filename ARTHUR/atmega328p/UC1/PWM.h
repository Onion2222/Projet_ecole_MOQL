/*
 * PWM.h
 *
 * Created: 23/03/2020 22:51:02
 *  Author: Onion
 */ 


void PWM_init();

void PWM_setA(uint16_t OCR);

void PWM_setB(uint16_t OCR);

void PWM_start();
void PWM_stop();