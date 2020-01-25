/*
 * sleep_mode.c
 *
 *  Created on: Jan 15, 2020
 *      Author: eng-m
 */


#include "sleep_mode.h"
#include "common_macros.h"
#include "registers.h"

#define ENABLE_SLEEP (0x80)
/*
 * Function: CPU_sleepInit
 * Description: enable the CPU to enter in the sleep mode
 */
void CPU_sleepInit(void)
{
	/* enable sleep mode first */

	SET_BIT(MCUCR,ENABLE_SLEEP);

}
/*
 * Function: CPU_sleep
 * Description:  the SLEEP instruction makes the MCU enter Idle mode with allowing to
 * some modules to work like uart, spi....
 */
void CPU_sleep(void)
{

	/* enter to sleep mode by SLEEP asm. instruction */
	asm("sleep");
}
