/*
 * ADC.h
 *
 * Created: 24/03/2020 21:20:38
 *  Author: onion
 */ 



void ADC_init();

void ADC_start();
void ADC_stop();

uint16_t ADC_getMesure();

uint16_t traitement(uint16_t adc);