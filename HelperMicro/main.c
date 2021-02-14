/*
 * main.c
 *
 *  Created on: Feb 13, 2021
 *      Author: Mostafa Alaa
 */
#include "std_types.h"
#include "DIO_interface.h"
#include "usart_driver.h"
#include <util/delay.h>


#define INC_SPEED				PIN1
#define DEC_SPEED				PIN2
#define GAS_PEDAL				PIN0
#define BRAKES					PIN3
#define UNPRESS					PIN4

#define GAS_UART				((uint8)0xFF)
#define BRAKES_UART				((uint8)0xFE)
#define UNPRESS_UART			((uint8)0xFD)


void System_init();


int main(void)
{
	/* System Initialization */
	System_init();
	uint8 speed = 0;
	usart_putc(speed);

	/* System Super Loop */
	while(1)
	{
		/* Increase speed button */
		if(!DIO_u8GetPinValue(0, INC_SPEED))
		{
			_delay_ms(30);
			while(!DIO_u8GetPinValue(0, INC_SPEED))
			{
				speed++;
				usart_putc(speed);
				_delay_ms(300);
			}
		}

		/* Decrease speed button */
		if(!DIO_u8GetPinValue(0, DEC_SPEED))
		{
			_delay_ms(30);
			while(!DIO_u8GetPinValue(0, DEC_SPEED))
			{
				speed--;
				usart_putc(speed);
				_delay_ms(300);
			}
		}

		/* Gas pressed button */
		if(!DIO_u8GetPinValue(0, GAS_PEDAL))
		{
			_delay_ms(30);
			if(!DIO_u8GetPinValue(0, GAS_PEDAL))
			{
				usart_putc(GAS_UART);
				while(!DIO_u8GetPinValue(0, GAS_PEDAL));
			}
		}

		/* Brakes pressed button */
		if(!DIO_u8GetPinValue(0, BRAKES))
		{
			_delay_ms(30);
			if(!DIO_u8GetPinValue(0, BRAKES))
			{
				usart_putc(BRAKES_UART);
				while(!DIO_u8GetPinValue(0, BRAKES));
			}
		}

		/* Pedal unpressed */
		if(!DIO_u8GetPinValue(0, UNPRESS))
		{
			_delay_ms(30);
			if(!DIO_u8GetPinValue(0, UNPRESS))
			{
				usart_putc(UNPRESS_UART);
				while(!DIO_u8GetPinValue(0, UNPRESS));
			}
		}
	}

	return 0;
}


/* System Initialization Function */
void System_init(void)
{
	DIO_VidSetPinDirection(0, PIN0, INPUT);	/* PINA0 Gas Pedal Button */
	DIO_VidSetPinDirection(0, PIN1, INPUT); /* PINA1 Increase Speed Button */
	DIO_VidSetPinDirection(0, PIN2, INPUT); /* PINA2 Decrease Speed Button */
	DIO_VidSetPinDirection(0, PIN3, INPUT); /* PINA3 Brakes Pedal Button */
	DIO_VidSetPinDirection(0, PIN4, INPUT); /* PINA4 NO Pedal Pressed Button */

	/* Enable internal Pull-up */
	DIO_VidSetPinValue(0, PIN0, HIGH);
	DIO_VidSetPinValue(0, PIN1, HIGH);
	DIO_VidSetPinValue(0, PIN2, HIGH);
	DIO_VidSetPinValue(0, PIN3, HIGH);
	DIO_VidSetPinValue(0, PIN4, HIGH);

	usart_init(9600); /* USART Initialization */
	usart_puts("System Started...\r\n");
}







