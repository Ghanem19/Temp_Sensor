/*
 * sleep_mode.h
 *
 *  Created on: Jan 15, 2020
 *      Author: eng-m
 */

#ifndef MCAL_SLEEP_SLEEP_MODE_H_
#define MCAL_SLEEP_SLEEP_MODE_H_
/*
 * Function: CPU_sleepInit
 * Description: enable the CPU to enter in the sleep mode
 */
void CPU_sleepInit(void);
/*
 * Function: CPU_sleep
 * Description:  the SLEEP instruction makes the MCU enter Idle mode with allowing to
 * some modules to work like uart, spi....
 */
void CPU_sleep(void);

#endif /* MCAL_SLEEP_SLEEP_MODE_H_ */
