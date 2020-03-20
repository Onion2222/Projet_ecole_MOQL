/*
 * UC2.c
 *
 * Created: 20/03/2020 15:53:57
 * Author : Onion
 */ 

#include <avr/io.h>

int volatile var=0;

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		for (var=0;var<500;var++)
		{
			if(var==200){
				var=5;
			}
				
		}
    }
}

