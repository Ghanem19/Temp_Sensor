/*
 * adc.c
 *
 *  Created on: Jan 25, 2020
 *      Author: eng-m
 */

#include "common_retval.h"
#include "common_macros.h"
#include "registers.h"
#include "Dio.h"
#include "adc.h"

/* ADMUX BITS */
#define	MUX0_0  	BIT0
#define	MUX1_1 		BIT1
#define	MUX2_2  	BIT2
#define	MUX3_3 		BIT3
#define	MUX4_4  	BIT4
#define	ADLAR		BIT5
#define	REFS0	 	BIT6
#define	REFS1		BIT7
/* ADCSRA BITS */
#define ADPS0		BIT0
#define ADPS1		BIT1
#define ADPS2		BIT2
#define ADIE 		BIT3
#define ADIF 		BIT4
#define ADATE		BIT5
#define ADSC 		BIT6
#define ADEN		BIT7

#define VREF_BITS		(REFS0|REFS1)
#define TRIGG_SRC_BITS	(BIT7 | BIT6 | BIT5)
#define PRESCALER_BITS  (BIT2 | BIT1 | BIT0)
#define CHANNEL_BITS  (BIT3 | BIT2 | BIT1 | BIT0)
#define ADC_NUM_CHANNEL 8

enum_initState_t gaenum_adcInitState[ADC_NUM_CHANNEL] = {0};
uint8 gu8_adcResolution = 0;
uint8 gu8_dataAdjustment = 0;
ERR_STATUS Adc_Init(str_adcCfg_t *str_adcCfg)
{
	ERR_STATUS adcInitErr = ERR_SUCCESS;
	DIO_Cfg_s str_adcIOPins;
	const uint8 au8_vrefMaskTable[] = {0x0, 0x40, 0xC0};
	const uint8 au8_triggerSrsMaskTable[] = {0x0,0x0,0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0};

	if(str_adcCfg == NULL)
	{
		adcInitErr = ERR_NULL_PTR;
	}
	else
	{

		if(str_adcCfg -> u8_channelNo > ADC_CHANNEL_7)
		{
			adcInitErr = ERR_INVALID_ARGU;
		}
		else
		{
			if(gaenum_adcInitState[str_adcCfg -> u8_channelNo] == INITIALIZED)
			{
				adcInitErr = ERR_ALREADY_INITIALIZED;
			}
			else
			{
				str_adcIOPins.GPIO = GPIOA;
				str_adcIOPins.dir = INPUT;
				str_adcIOPins.pins = str_adcCfg -> u8_channelNo;

				if(E_NOK == DIO_init(&str_adcIOPins))
				{
					adcInitErr = ERR_DIO;
				}
				else
				{
					if(str_adcCfg->u8_refVoltage > ADC_VREF_EXT)
					{
						adcInitErr = ERR_INVALID_ARGU;
					}
					else
					{
						ASSIGN_BIT(ADMUX,VREF_BITS,au8_vrefMaskTable[str_adcCfg->u8_refVoltage]);
						if(str_adcCfg->u8_interruptMode > ADC_INTERRUPT)
						{
							adcInitErr = ERR_INVALID_ARGU;
						}
						else
						{
							ASSIGN_BIT(ADCSRA,ADIE,str_adcCfg->u8_interruptMode*HIGH);

							if(str_adcCfg->u8_dataAdjust > ADC_LEFT_ADGJUST)
							{
								adcInitErr = ERR_INVALID_ARGU;
							}
							else
							{

								ASSIGN_BIT(ADMUX, ADLAR, str_adcCfg->u8_dataAdjust*HIGH);
								gu8_dataAdjustment = str_adcCfg->u8_dataAdjust;
								if(str_adcCfg->u8_resolution > ADC_RESOLUTION_10)
								{
									adcInitErr = ERR_INVALID_ARGU;
								}
								else
								{
									gu8_adcResolution = str_adcCfg->u8_resolution;

								}

							}
						}
					}
				}
			}
		}
	}


	if(adcInitErr == ERR_SUCCESS)
	{
		if(str_adcCfg->u8_prescaler > ADC_PRESCALER_128)
		{
			adcInitErr = ERR_INVALID_ARGU;
		}
		else
		{
			ASSIGN_BIT(ADCSRA ,PRESCALER_BITS, str_adcCfg->u8_prescaler);
			if(str_adcCfg->u8_autoTriggSource == ADC_SINGLE_CONV)
			{
				CLR_BIT(ADCSRA, ADATE);
				gaenum_adcInitState[str_adcCfg->u8_channelNo] = INITIALIZED;

			}
			else if((str_adcCfg->u8_autoTriggSource >= ADC_FREE_RUNNING) && \
					(str_adcCfg->u8_autoTriggSource <= ADC_TIMER1_ICP_TRIGG))
			{
				SET_BIT(ADCSRA, ADATE);
				ASSIGN_BIT(SFIOR,TRIGG_SRC_BITS, au8_triggerSrsMaskTable[str_adcCfg->u8_autoTriggSource]);
				gaenum_adcInitState[str_adcCfg->u8_channelNo] = INITIALIZED;

			}
			else
			{
				adcInitErr = ERR_INVALID_ARGU;
			}

		}
	}

			return adcInitErr;
}

void Adc_Enable(void)
{
	SET_BIT(ADCSRA,ADEN);
}

ERR_STATUS Adc_StartConversion(uint8 u8_channelNumber)
{
	if(gaenum_adcInitState[u8_channelNumber] == UNINITIALIZED)
	{

	}
	else
	{
		if(u8_channelNumber >= ADC_NUM_CHANNEL)
		{

		}
		else
		{
			ASSIGN_BIT(ADMUX, CHANNEL_BITS, u8_channelNumber);
			SET_BIT(ADCSRA,ADSC);
		}

	}
}


ERR_STATUS Adc_getConvertedValue(uint16 *pu16_convertedData)
{
	ERR_STATUS getValueErr = ERR_SUCCESS;

	if(pu16_convertedData == NULL)
	{

	}
	else
	{
		switch(gu8_adcResolution)
		{
			case(ADC_RESOLUTION_8):
				switch(gu8_dataAdjustment)
				{
					case(ADC_RIGHT_ADGJUST):
						*pu16_convertedData = ADCL;
							break;
					case(ADC_LEFT_ADGJUST):
						*pu16_convertedData = ADCH;
							break;
					default:
						getValueErr = ERR_LOGICAL;

				}

					break;
			case(ADC_RESOLUTION_10):
				switch(gu8_dataAdjustment)
				{
					case(ADC_RIGHT_ADGJUST):
						*pu16_convertedData = ADCL;
						*pu16_convertedData |= (ADCH<<8);

							break;
					case(ADC_LEFT_ADGJUST):
						*pu16_convertedData = ADCL>>6;
						*pu16_convertedData |= (ADCH>>6);
							break;
					default:
						getValueErr = ERR_LOGICAL;
				}
			break;
			default:
				getValueErr = ERR_LOGICAL;
				break;
		}
	}
}


void Adc_Disable(void)
{
	CLR_BIT(ADCSRA,ADEN);

}
