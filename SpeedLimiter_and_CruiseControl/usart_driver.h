#include <avr/io.h>

#define       FCLK_SYSTEM     8000000UL

void          usart_init(unsigned short int baudrate);
void          usart_putc(unsigned char data);
unsigned char usart_getc(uint8_t *data);
unsigned char usart_getc_unblock(uint8_t *data);
void          usart_puts(char *str);
