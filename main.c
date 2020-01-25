/*
 * main.c
 *
 *  Created on: Jan 21, 2020
 *      Author: Ghanem
 */

#include "common_retval.h"
#include "interrupt.h"
#include "lcd.h"
#include "registers.h"
#include "common_macros.h"
#include <stdlib.h>
#include "adc.h"

int main(void)
{
	PORTA_DIR = 0x00;
	LCD_init();
	uint8 buff[16] ;
	uint16 data = 0;
	str_adcCfg_t adcCfg =
		{
				ADC_CHANNEL_0,
				ADC_VREF_INT,
				ADC_RIGHT_ADGJUST,
				ADC_PRESCALER_128,
				ADC_RESOLUTION_10,
				ADC_POLLING,
				ADC_FREE_RUNNING,
				NULL

		};
	Adc_Init(&adcCfg);
	Adc_Enable();

	while(1)
	{
		Adc_StartConversion(ADC_CHANNEL_0);
		while( GET_BIT(ADCSRA,0x01) == 0 ){}
		Adc_getConvertedValue(&data);
		itoa(data,buff,10);
		LCD_sendString_RowCol(0,4,buff);
	}

}
