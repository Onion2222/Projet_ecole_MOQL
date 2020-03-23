/*
 * UART.h
 *
 * Created: 23/03/2020 17:55:45
 *  Author: onion
 */ 


void USART0_init(uint16_t ubrr);

void USART0_sendByte(unsigned char data);

void USART0_sendString(char* data);