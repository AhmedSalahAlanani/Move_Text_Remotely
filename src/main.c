/*
 * main.c
 *
 *  Created on: Sep 2, 2020
 *      Author: Ahmed Salah
 */
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "RCC_interface.h"
#include "LEDMRX_interface.h"
#include "IR_NEC_interface.h"


//Macros
#define LEDMRX_WIDTH		(8)
#define LEDMRX_HEIGHT		(8)
#define TEXT_WIDTH(text,type)	(sizeof(text)/sizeof(type))


//Global Variables
volatile u8 offset;
volatile u8 Shift ;
static const u8 SampleText [] = {	 0,  0,  0,  0,  0, 0,  0,  0,
									 0, 68, 74, 74, 82, 98, 0,100,
									82, 82,124, 64,  0,124, 8,120,
									 8,120,  0,124, 20, 20,24,  0,
								   126, 64,  0, 56, 84, 84,84, 88,
								     0,  0,
									 0,  2,  2,126,  2,  2, 0,  0,
								   124, 84, 84, 84,  0, 68,40, 16,
								    40, 68,  0,  4,126, 68,68, 64,
									 0,  0,  0,  0,  0,  0, 0, 0 	 };


void MoveRight(void);
void MoveLeft (void);
void MoveUp	  (void);
void MoveDown (void);


int main (void)
{
	static   u8 Frame [LEDMRX_WIDTH];
	volatile u8 DisplayOn = 0;
	volatile u8 Change;

	MRCC_voidInitSysClock ();
	MRCC_voidEnableClock  (RCC_APB2,RCC_APB2_IOPAEN);
	MRCC_voidEnableClock  (RCC_APB2,RCC_APB2_IOPBEN);
	MRCC_voidEnableClock  (RCC_APB2,RCC_APB2_AFIOEN);
	MRCC_voidEnableClock  (RCC_APB1,RCC_APB1_TIM2EN);


	HLEDMRX_voidInit();
	HIR_NEC_voidInit();

	while(1)
	{
		switch( HIR_NEC_u8ReadReceivedFrame() )
		{
			case IR_NEC_NO_BUTTON_PRESSED:
				asm volatile ("NOP");
				break;

			case IR_NEC_BUTTON_UP_PRESSED:
				if(DisplayOn)
				{
					MoveUp();
					Change = 1;
				}
				break;

			case IR_NEC_BUTTON_DOWN_PRESSED:
				if(DisplayOn)
				{
					MoveDown();
					Change = 1;
				}
				break;

			case IR_NEC_BUTTON_RIGHT_PRESSED:
				if(DisplayOn)
				{
					MoveRight();
					Change = 1;
				}
				break;

			case IR_NEC_BUTTON_LEFT_PRESSED:
				if(DisplayOn)
				{
					MoveLeft();
					Change = 1;
				}
				break;

			case IR_NEC_BUTTON_OK_PRESSED:
				if(DisplayOn)
				{
					HLEDMRX_voidTurnOffDisplay();
					DisplayOn = 0;
				}
				else
				{
					offset    = LEDMRX_WIDTH;
					Shift     = LEDMRX_HEIGHT;
					Change    = 1;
					DisplayOn = 1;
				}
				break;
		}

		if(DisplayOn)
		{
			if(Change)
			{
				for(u8 i=0; i<LEDMRX_WIDTH ; i++)
				{
					Frame[i] = SampleText[i+offset];

					if(Shift > LEDMRX_HEIGHT)
					{
						Frame[i] <<= Shift % LEDMRX_HEIGHT;
					}
					else if(Shift < 8)
					{
						Frame[i] >>= ((LEDMRX_HEIGHT- Shift)%LEDMRX_HEIGHT);
					}
				}

				Change = 0;
			}

			/*	Display text	*/
			HLEDMRX_voidDisplayFrame(Frame);
		}

	}
}

void MoveRight(void)
{
	offset--;

	if(offset < 1)
	{
		offset = (TEXT_WIDTH(SampleText,u8) - LEDMRX_WIDTH);
	}
}

void MoveLeft(void)
{
	offset++;

	if(offset > (TEXT_WIDTH(SampleText,u8) - LEDMRX_WIDTH) )
	{
		offset = 1;
	}
}

void MoveUp(void)
{
	Shift--;

	if(Shift < 1)
	{
		Shift = (2*LEDMRX_HEIGHT) - 1;
	}
}

void MoveDown (void)
{
	Shift++;

	if(Shift > ((2*LEDMRX_HEIGHT) - 1) )
	{
		Shift = 1;
	}
}

