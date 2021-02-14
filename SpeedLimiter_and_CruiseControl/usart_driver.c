#include <avr/io.h>
#include "usart_driver.h"

void usart_init(unsigned short int baudrate)
{
	//Set baud rate
	unsigned short int UBRRVAL=(FCLK_SYSTEM/(baudrate*16UL))-1;
	UBRR0L=UBRRVAL; 			//low byte
	UBRR0H=(UBRRVAL>>8); 	//high byte

	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	//UCSRC=(1<<URSEL)|(0<<UMSEL)|(0<<UPM1)|(0<<UPM0)|(0<<USBS)|(0<<UCSZ2)|(1<<UCSZ1)|(1<<UCSZ0);

	//Enable Transmitter and Receiver
	//UCSRB=(1<<RXEN)|(1<<TXEN);

	UCSR0B= (1<<RXEN) | (1<<TXEN);                  // Enable Receiver and Transmitter
	UCSR0C= (1<<UCSZ1) | (1<<UCSZ0);                // Asynchronous mode 8-bit data and 1-stop bit
	UCSR0A= 0x00;
}
unsigned char usart_getc( uint8_t * data )
{
	/* Wait for data to be received */
	while(!(UCSR0A&(1<<RXC)));  // Wait till the data is received
	*data = UDR0;
	return 1;
}

unsigned char usart_getc_unblock( uint8_t * data )
{
	uint8_t status=0;
	/* Wait for data to be received */
	if(UCSR0A & (1<<RXC))
	{
	/* Get and return received data from buffer */
	*data = UDR0;
	status =1;
	}
	return status;
}

void usart_putc( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void usart_puts( char* str )
{
	short int i=0;
	while(str[i])
	{
		usart_putc(str[i]);
		i++;
	}
}
