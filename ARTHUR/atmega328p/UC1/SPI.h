/*
 * SPI.h
 *
 * Created: 24/03/2020 13:11:39
 *  Author: Onion
 */ 


void SPI_init();

uint8_t SPI_MasterEnvoieReception(unsigned char data);
void SPI_MasterEnvoie(unsigned char data);