/*
 * timer1.h
 *
 * Created: 24/03/2020 19:34:46
 *  Author: onion
 */ 

void PWM_timer1_init();

void PWM_timer1_start();

void PWM_timer1_stop();

void PWM_timer1_set(uint8_t ocr);
uint8_t PWM_timer1_get();

uint8_t PWM_timer1_isStopped();

void PWM_timer1_incr();

void PWM_timer1_decr();

