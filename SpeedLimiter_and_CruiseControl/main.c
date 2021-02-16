/*
 * main.c
 *
 * Description: Speed Limiter and Cruise Control Project
 */

#include"std_types.h"
#include"BIT_MATH.h"
#include "DIO_interface.h"
#include "usart_driver.h"
#include "lcd.h"
#include <stdlib.h>
#include <stdio.h>

#include "FreeRTOS/Src/FreeRTOS.h"
#include "FreeRTOS/Src/task.h"
#include "FreeRTOS/Src/queue.h"
#include "FreeRTOS/Src/semphr.h"
#include "FreeRTOS/Src/event_groups.h"

/* System Initialization Function */
void System_init(void);

/* Tasks */
void T_UserInput(void *pvInitData);
void T_Alarm(void *pvInitData);
void T_Control(void *pvInitData);
void T_Uart(void *pvInitData);
void T_Display(void *pvInitData);


/* System states */
#define MAIN_CRUISE					(1)
#define CRUISE_CONTROL				(2)
#define SPEED_LIMITER				(3)
#define ALARM						(4)

/* IO Pins (All connected to port A) */
/* Buttons */
#define BTN_CC						PIN0	/* PINA0 */
#define BTN_SL						PIN1	/* PINA1 */
#define BTN_STANDBY					PIN2	/* PINA2 */
#define BTN_INC						PIN3	/* PINA3 */
#define BTN_DEC						PIN4	/* PINA4 */
/* Outputs */
#define LED							PIN5	/* PINA5 */
#define BUZZER						PIN6	/* PINA6 */

/* Display Events */
#define E_MAIN_SCR					(1 << 0)	/* Main cruise screen event */
#define E_SL_SCR					(1 << 1)    /* Speed Limiter screen event */
#define E_CC_SCR					(1 << 2)    /* Cruise Control screen event */
#define E_ALARM_SCR					(1 << 3)    /* Alarm screen event */
#define E_V_SPEED					(1 << 4)    /* Vehicle speed update event */
#define E_C_SPEED					(1 << 5)    /* Control speed update event */
/* Control Events */
#define E_ALARM						(1 << 6)	/* Alarm trigger event */

/* Speed Limiter and Cruise Control Statuses */
#define OFF							(0)
#define ON							(1)
#define STANDBY						(2)

/* Gas and Brakes pedals statuses */
#define UNPRESSED					(0)
#define PRESSED						(1)

/* Gas and Brakes UART symbols */
#define GAS_UART				((uint8)0xFF)
#define BRAKES_UART				((uint8)0xFE)
#define UNPRESS_UART			((uint8)0xFD)


/*
 * Structure Name: 	SysParam
 *
 * Description:		Contains the system parameters
 *
 */
typedef struct
{
	uint8 SysState;			/* Current state of the system */
	uint8 VehicleSpeed;		/* Current vehicle speed */
	uint8 ControlSpeed;		/* Speed set for either the speed limiter or the cruise control */
	uint8 CruiseControl;	/* Status of cruise control  */
	uint8 SpeedLimiter;		/* Status of speed limiter */
	uint8 GasPedal;			/* Status of the gas pedal (Pressed or not) */
	uint8 Brakes;			/* Status of the brakes (Pressed or not) */
}SysParam;


/* SysParam object initialization */
SysParam Param =
{
		.SysState = MAIN_CRUISE,
		.ControlSpeed = 0,
		.CruiseControl = OFF,
		.SpeedLimiter = OFF,
		.GasPedal = UNPRESSED,
		.Brakes = UNPRESSED
};

/* Events Group Handle */
EventGroupHandle_t		egEvents;
EventBits_t				ebEvents;


/* Control binary semaphore */
SemaphoreHandle_t 		bsControl;


int main(void)
{
	System_init();

	bsControl=xSemaphoreCreateBinary();

	egEvents = xEventGroupCreate();

	xTaskCreate(T_UserInput, NULL, 200, NULL, 2, NULL);
	xTaskCreate(T_Alarm, NULL, 200, NULL, 5, NULL);
	xTaskCreate(T_Control, NULL, 200, NULL, 4, NULL);
	xTaskCreate(T_Uart, NULL, 300, NULL, 3, NULL);
	xTaskCreate(T_Display, NULL, 400, NULL, 1, NULL);

	/* Display Initialization */
	xEventGroupSetBits(egEvents, E_MAIN_SCR);

	vTaskStartScheduler();
}


/*
 * ---------------------------------------------------------------------------------
 * -- Tasks Definitions
 * ---------------------------------------------------------------------------------
 */

/*
 * Task Name: 		T_UserInput
 *
 * Description: 	Checks the driver's input by any of the five speed limiter and cruise
 * 					control buttons.
 *
 */
void T_UserInput(void *pvInitData)
{
	usart_puts("T_UserInput Started...\r\n");

	while(1)
	{
		/* Increase speed button */
		if(!DIO_u8GetPinValue(0, BTN_INC))
		{
			vTaskDelay(30);
			{
				if(!DIO_u8GetPinValue(0, BTN_INC))
				{
					if(Param.SpeedLimiter == STANDBY)
					{
						/* Resume the Speed Limiter from standby */
						Param.SysState = SPEED_LIMITER;
						Param.SpeedLimiter = ON;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_SL_SCR | E_C_SPEED);
					}
					else if(Param.CruiseControl == STANDBY)
					{
						/* Resume the Cruise Control from standby with saved control speed */
						Param.SysState = CRUISE_CONTROL;
						Param.CruiseControl = ON;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_CC_SCR | E_C_SPEED);
					}
					else if(Param.SysState != MAIN_CRUISE)
					{
						/* Increment control speed */
						Param.ControlSpeed++;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_C_SPEED);
					}

					while(!DIO_u8GetPinValue(0, BTN_INC))
					{
						vTaskDelay(150);
					}
				}
			}
		}

		/* Decrease speed button */
		if(!DIO_u8GetPinValue(0, BTN_DEC))
		{
			vTaskDelay(30);
			{
				if(!DIO_u8GetPinValue(0, BTN_DEC))
				{
					if(Param.SpeedLimiter == STANDBY)
					{
						/* Resume the Speed Limiter from standby */
						Param.SysState = SPEED_LIMITER;
						Param.SpeedLimiter = ON;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_SL_SCR | E_C_SPEED);
					}
					else if(Param.CruiseControl == STANDBY)
					{
						/*
						 * Resume the Cruise Control from standby with
						 * control speed = current vehicle speed
						 */
						Param.SysState = CRUISE_CONTROL;
						Param.CruiseControl = ON;
						Param.ControlSpeed = Param.VehicleSpeed;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_CC_SCR | E_C_SPEED);
					}
					else if(Param.SysState != MAIN_CRUISE)
					{
						/* Decrement control speed */
						Param.ControlSpeed--;
						xSemaphoreGive(bsControl);
						xEventGroupSetBits(egEvents, E_C_SPEED);
					}

					while(!DIO_u8GetPinValue(0, BTN_DEC))
					{
						vTaskDelay(150);
					}
				}
			}
		}

		/* Speed Limiter button */
		if(!DIO_u8GetPinValue(0, BTN_SL))
		{
			vTaskDelay(30);
			if(!DIO_u8GetPinValue(0, BTN_SL))
			{
				if(Param.SpeedLimiter == OFF)
				{
					/* Turn Speed Limiter ON */
					Param.SysState = SPEED_LIMITER;
					Param.CruiseControl = OFF;
					Param.SpeedLimiter = ON;
					Param.ControlSpeed = 0;
					xSemaphoreGive(bsControl);
					xEventGroupSetBits(egEvents, E_SL_SCR);
				}
				else
				{
					/* Turn Speed Limiter OFF */
					Param.SysState = MAIN_CRUISE;
					Param.SpeedLimiter = OFF;
					xEventGroupSetBits(egEvents, E_MAIN_SCR);
				}
			}
			while(!DIO_u8GetPinValue(0, BTN_SL))
			{
				vTaskDelay(150);
			}
		}

		/* Cruise Control button*/
		if(!DIO_u8GetPinValue(0, BTN_CC))
		{
			vTaskDelay(30);
			if(!DIO_u8GetPinValue(0, BTN_CC))
			{
				if(Param.CruiseControl == OFF)
				{
					/* Turn Cruise Control ON */
					Param.SysState = CRUISE_CONTROL;
					Param.SpeedLimiter = OFF;
					Param.CruiseControl = ON;
					Param.ControlSpeed = 0;
					xSemaphoreGive(bsControl);
					xEventGroupSetBits(egEvents, E_CC_SCR);
				}
				else
				{
					/* Turn Cruise Control OFF */
					Param.SysState = MAIN_CRUISE;
					Param.CruiseControl = OFF;
					xEventGroupSetBits(egEvents, E_MAIN_SCR);
				}
			}
			while(!DIO_u8GetPinValue(0, BTN_CC))
			{
				vTaskDelay(150);
			}
		}

		/* Standby button */
		if(!DIO_u8GetPinValue(0, BTN_STANDBY))
		{
			vTaskDelay(30);
			if(!DIO_u8GetPinValue(0, BTN_STANDBY))
			{
				if(Param.SpeedLimiter == ON)
				{
					/* Put Speed Limiter into standby */
					Param.SpeedLimiter = STANDBY;
					Param.SysState = MAIN_CRUISE;
					xEventGroupSetBits(egEvents, E_MAIN_SCR);
				}
				else if(Param.CruiseControl == ON)
				{
					/* Put Speed Limiter into standby */
					Param.CruiseControl = STANDBY;
					Param.SysState = MAIN_CRUISE;
					xEventGroupSetBits(egEvents, E_MAIN_SCR);
				}
			}
			while(!DIO_u8GetPinValue(0, BTN_STANDBY))
			{
				vTaskDelay(150);
			}
		}


		vTaskDelay(150);
	}
}


/*
 * Task Name:		T_Alarm
 *
 * Description:		Flashes a led (and a buzzer in potentially dangerous cases) to Alarm
 * 					the driver when the vehicle speed exceeds the speed limit.
 * 					(The dangerous case is when the vehicle speed exceeds the speed limit
 * 					 while the driver is not pressing the gas pedal)
 *
 */
void T_Alarm(void *pvInitData)
{
	usart_puts("T_Alarm Started...\r\n");
	while(1)
	{
		ebEvents = xEventGroupWaitBits(egEvents,
				E_ALARM,
				0,
				0,
				portMAX_DELAY);

		if(ebEvents & E_ALARM)
		{
			/* Turn Led ON */
			DIO_VidSetPinValue(0, LED, 0);

			if(Param.GasPedal == UNPRESSED)
			{
				/* Turn Buzzer ON */
				DIO_VidSetPinValue(0, BUZZER, 1);
			}

			vTaskDelay(500);

			/* Turn OFF Led and Buzzer */
			DIO_VidSetPinValue(0, LED, 1);
			DIO_VidSetPinValue(0, BUZZER, 0);

			vTaskDelay(500);
		}
	}
}


/*
 * Task Name:		T_Control
 *
 * Description:		Determine the control needed when either Speed Limiter or Cruise
 * 					Control are ON, and sends control action over UART.
 *
 */
void T_Control(void *pvInitData)
{
	usart_puts("T_Check Started...\r\n");

	while(1)
	{
		if(xSemaphoreTake(bsControl, portMAX_DELAY))
		{
			/* Speed Limiter control */
			if(Param.SpeedLimiter == ON)
			{
				/*
				 * The system parameter ControlSpeed = 0 indicates that Speed Limiter
				 * is just turned on. In this case the ControlSpeed is set to the current
				 * VehicleSpeed and control action is sent over UART to increase the gas
				 * pedal's resistance.
				 */
				if(Param.ControlSpeed == 0)
				{
					Param.ControlSpeed = Param.VehicleSpeed;
					xEventGroupSetBits(egEvents, E_C_SPEED);
					usart_puts("Increase gas pedal resistance!\r\n");
				}

				/*
				 * Whenever the VehicleSpeed is equal to the ControlSpeed, control action
				 * is sent over UART to increase the gas pedal's resistance.
				 */
				else if(Param.VehicleSpeed == Param.ControlSpeed)
				{
					usart_puts("Increase gas pedal resistance!\r\n");
				}

				/* Whenever the VehicleSpeed exceeds the ControlSpeed, alarm is triggered */
				else if(Param.VehicleSpeed > Param.ControlSpeed)
				{
					Param.SysState = ALARM;
					xEventGroupSetBits(egEvents, E_ALARM | E_ALARM_SCR);
				}
				/*
				 * When alarm is on and VehicleSpeed gets smaller than ControlSpeed, alarm
				 * is turned off.
				 */
				else if(Param.SysState == ALARM)
				{
					Param.SysState = MAIN_CRUISE;
					xEventGroupSetBits(egEvents, E_SL_SCR);
					xEventGroupClearBits(egEvents, E_ALARM);
				}
			}

			/* Cruise Control control */
			else if(Param.CruiseControl == ON)
			{
				/* Control action buffer */
				char controlAction[10];

				/*
				 * The system parameter ControlSpeed = 0 indicates that Cruise Control
				 * is just turned on. In this case the ControlSpeed is set to the current
				 * VehicleSpeed and control action is sent over UART to adjust VehicleSpeed.
				 */
				if(Param.ControlSpeed == 0)
				{
					Param.ControlSpeed = Param.VehicleSpeed;
					xEventGroupSetBits(egEvents, E_C_SPEED);
					sprintf(controlAction, "%d\r\n", Param.ControlSpeed - Param.VehicleSpeed);
					usart_puts(controlAction);
				}

				/* Whenever the VehicleSpeed exceeds the ControlSpeed, alarm is triggered */
				else if(Param.VehicleSpeed > Param.ControlSpeed)
				{
					Param.SysState = ALARM;
					xEventGroupSetBits(egEvents, E_ALARM | E_ALARM_SCR);
				}
				/*
				 * When alarm is on and VehicleSpeed gets smaller than ControlSpeed, alarm
				 * is turned off.
				 */
				else if(Param.SysState == ALARM)
				{
					Param.SysState = MAIN_CRUISE;
					xEventGroupSetBits(egEvents, E_SL_SCR);
					xEventGroupClearBits(egEvents, E_ALARM);
				}
				/* Control action is sent over UART to adjust VehicleSpeed */
				else
				{
					sprintf(controlAction, "%d\r\n", Param.ControlSpeed - Param.VehicleSpeed);
					usart_puts(controlAction);
				}

				/* Whenever the Brakes is pressed, Cruise Control is put into standby */
				if(Param.Brakes == PRESSED)
				{
					Param.SysState = MAIN_CRUISE;
					Param.CruiseControl = STANDBY;
					xEventGroupSetBits(egEvents, E_MAIN_SCR);
				}
			}
		}
	}
}


/*
 * Task Name:		T_Uart
 *
 * Description:		Receives the vehicle speed, and the press/unpress of gas and brakes.
 *
 */
void T_Uart(void *pvInitData)
{
	usart_puts("T_Terminal Started...\r\n");
	uint8 chr;

	while(1)
	{
		if(!usart_getc_unblock(&chr))
		{
			vTaskDelay(50);
			continue;
		}

		if(chr == GAS_UART)
		{
			Param.GasPedal = PRESSED;
			xSemaphoreGive(bsControl);
		}

		/* Brakes is only used to put Cruise Control into standby */
		else if(chr == BRAKES_UART && Param.CruiseControl == ON)
		{
			Param.Brakes = PRESSED;
			xSemaphoreGive(bsControl);
		}

		else if(chr == UNPRESS_UART)
		{
			Param.GasPedal = UNPRESSED;
			xSemaphoreGive(bsControl);
		}

		else
		{
			Param.VehicleSpeed = chr;
			xEventGroupSetBits(egEvents, E_V_SPEED);
			xSemaphoreGive(bsControl);
		}

		vTaskDelay(50);
	}
}


/*
 * Task Name:		T_Display
 *
 * Description:		Displays the speed of the vehicles.
 *
 */
void T_Display(void *pvInitData)
{
	usart_puts("T_Display Started...\r\n");

	while(1)
	{
		ebEvents = xEventGroupWaitBits(egEvents,
				(E_MAIN_SCR | E_SL_SCR | E_CC_SCR | E_ALARM_SCR | E_V_SPEED | E_C_SPEED),
				1,
				0,
				portMAX_DELAY);

		if(ebEvents & E_MAIN_SCR)
		{
			if(Param.SpeedLimiter == STANDBY || Param.CruiseControl == STANDBY)
			{
				LCD_displayStringRowColumn(0, 14, "STANDBY");
			}
			else
			{
				LCD_clearScreen();
				xEventGroupSetBits(egEvents, E_V_SPEED);
			}
		}
		else if(ebEvents & E_SL_SCR)
		{
			LCD_clearScreen();
			xEventGroupSetBits(egEvents, E_V_SPEED);
			LCD_displayStringRowColumn(0, 0, "S.L");
		}
		else if(ebEvents & E_CC_SCR)
		{
			LCD_clearScreen();
			xEventGroupSetBits(egEvents, E_V_SPEED);
			LCD_displayStringRowColumn(0, 0, "C.C");
		}
		else if(ebEvents & E_ALARM_SCR)
		{
			LCD_displayStringRowColumn(0, 7, "ALARM");
		}

		if(ebEvents & E_V_SPEED)
		{
			LCD_goToRowColumn(1, 6);
			LCD_intgerToString(Param.VehicleSpeed);
		}

		if(ebEvents & E_C_SPEED)
		{
			LCD_goToRowColumn(0, 3);
			LCD_intgerToString(Param.ControlSpeed);
		}
	}
}


/*
 * Function Name:	System_init
 *
 * Description:		Initializes the peripherals and DIO pins needed by the system.
 *
 */
void System_init(void)
{
	/* DIOs Initialization */
	/* Five switches to control speed limiter and cruise control */
	DIO_VidSetPinDirection(0, BTN_CC, INPUT);
	DIO_VidSetPinDirection(0, BTN_SL, INPUT);
	DIO_VidSetPinDirection(0, BTN_STANDBY, INPUT);
	DIO_VidSetPinDirection(0, BTN_INC, INPUT);
	DIO_VidSetPinDirection(0, BTN_DEC, INPUT);
	/* Outputs */
	DIO_VidSetPinDirection(0, LED, OUTPUT);
	DIO_VidSetPinDirection(0, BUZZER, OUTPUT);

	/* UART Initialization */
	usart_init(9600);

	/* LCD Initialization */
	LCD_init();

	usart_puts("System Started...\r\n");
}



