
#include "std_types.h"
#include "BIT_MATH.h"
#include "DIO_register.h"

/*
loc_u8port=port name 
loc_u8pin =pin number
loc_u8state=pin direction
*/
void DIO_VidSetPinDirection(uint8 loc_u8port,uint8 loc_u8pin, uint8 loc_u8state){
	
	if(1==loc_u8state){
	    switch(loc_u8port){
		case 0:SET_BIT(DDRA,loc_u8pin);break;
		case 1:SET_BIT(DDRB,loc_u8pin);break;
		case 2:SET_BIT(DDRC,loc_u8pin);break;
		case 3:SET_BIT(DDRD,loc_u8pin);break;
	 }
	}
	else if(0==loc_u8state){
	    switch(loc_u8port){
		case 0:CLR_BIT(DDRA,loc_u8pin);break;
		case 1:CLR_BIT(DDRB,loc_u8pin);break;
		case 2:CLR_BIT(DDRC,loc_u8pin);break;
		case 3:CLR_BIT(DDRD,loc_u8pin);break;
	 }
	}
	
	
}
void DIO_VidSetPinValue(uint8 loc_u8port,uint8 loc_u8pin, uint8 loc_u8value){
	
	if(1==loc_u8value){
	    switch(loc_u8port){
		case 0:SET_BIT(PORTA,loc_u8pin);break;
		case 1:SET_BIT(PORTB,loc_u8pin);break;
		case 2:SET_BIT(PORTC,loc_u8pin);break;
		case 3:SET_BIT(PORTD,loc_u8pin);break;
	 }
	}
	else if(0==loc_u8value){
	    switch(loc_u8port){
		case 0:CLR_BIT(PORTA,loc_u8pin);break;
		case 1:CLR_BIT(PORTB,loc_u8pin);break;
		case 2:CLR_BIT(PORTC,loc_u8pin);break;
		case 3:CLR_BIT(PORTD,loc_u8pin);break;
	 }
	}
}
void DIO_VidSetPortDirection(uint8 loc_u8port,uint8 loc_u8value){
	

			switch(loc_u8port){

			case 0:DDRA=loc_u8value;break;
			case 1:DDRB=loc_u8value;break;
			case 2:DDRC=loc_u8value;break;
			case 3:DDRD=loc_u8value;break;

			}
		
}
void DIO_VidSetPortValue(uint8 loc_u8port,uint8 loc_u8value){
	
			switch(loc_u8port){

			case 0:PORTA=loc_u8value;break;
			case 1:PORTB=loc_u8value;break;
			case 2:PORTC=loc_u8value;break;
			case 3:PORTD=loc_u8value;break;

			}
	
}
uint8 DIO_u8GetPinValue(uint8 loc_u8port,uint8 loc_u8pin){
	uint8 VALUE;
	switch(loc_u8port){
		case 0:VALUE=GET_BIT(PINA,loc_u8pin); break;
		case 1:VALUE=GET_BIT(PINB,loc_u8pin); break;
		case 2:VALUE=GET_BIT(PINC,loc_u8pin); break;
		case 3:VALUE=GET_BIT(PIND,loc_u8pin); break;
	 }
	 return VALUE;
}

void DIO_VidTogBit(uint8 loc_u8port,uint8 LOC_u8BitNum){
	switch(loc_u8port){
		case 0: TOG_BIT(PORTA,LOC_u8BitNum); break;
		case 1: TOG_BIT(PORTB,LOC_u8BitNum); break;
		case 2: TOG_BIT(PORTC,LOC_u8BitNum); break;
		case 3: TOG_BIT(PORTD,LOC_u8BitNum); break;
	}	
}
