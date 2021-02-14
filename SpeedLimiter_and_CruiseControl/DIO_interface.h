#ifndef _DIO_INTERFACE_H_
#define _DIO_INTERFACE_H_

#define OUTPUT   1
#define INPUT    0
#define PORTA    0
#define PORTB    1
#define PORTC    2
#define PORTD    3
#define HIGH     1
#define LOW      0

//  loc_u8port=port name   loc_u8pin=pin number      loc_u8state=pin state (input or output)  //
//set the pin direction//
void DIO_VidSetPinDirection(uint8 loc_u8port,uint8 loc_u8pin, uint8 loc_u8state);
// set PIN value//
void DIO_VidSetPinValue(uint8 loc_u8port,uint8 loc_u8pin, uint8 loc_u8value);
//set the port direction//
void DIO_VidSetPortDirection(uint8 loc_u8port,uint8 loc_u8value);
// set port value//
void DIO_VidSetPortValue(uint8 loc_u8port,uint8 loc_u8value);
//get the value of the pin//
uint8 DIO_u8GetPinValue(uint8 loc_u8port,uint8 loc_u8pin);

void DIO_VidTogBit(uint8 loc_u8port,uint8 LOC_u8BitNum);

#endif