/*
 * lcd_cfg.h
 *
 *  Created on: Jan 22, 2020
 *      Author: eng-m
 */

#ifndef ECUAL_LCD_LCD_CFG_H_
#define ECUAL_LCD_LCD_CFG_H_
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "Dio.h"

/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
#define SUPER_LOOP (0U)
#define OS  	   (1U)

#define SYSTEM_TYPE SUPER_LOOP


/* RS (register selection pin ) */
#define LCD_RS_PIN	BIT1
#define LCD_RS_PORT	GPIOA

/* R/W pin (select read or write operation )*/
#define LCD_RW_PIN	BIT2
#define LCD_RW_PORT	GPIOA

/* E (enable pin of LCD ) */
#define LCD_EN_PIN	BIT3
#define LCD_EN_PORT	GPIOA
/* Data pins defines */
#define LCD_D4_PIN	BIT4
#define LCD_D5_PIN	BIT5
#define LCD_D6_PIN	BIT6
#define LCD_D7_PIN	BIT7


#define LCD_PORT GPIOA
#define LCD_PINS  (LCD_RS_PIN | LCD_RW_PIN |LCD_EN_PIN \
		| LCD_D4_PIN | LCD_D5_PIN | LCD_D6_PIN | LCD_D7_PIN)

#endif /* ECUAL_LCD_LCD_CFG_H_ */
