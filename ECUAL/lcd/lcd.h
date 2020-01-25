/*
 * lcd.h
 *
 *  Created on: Jan 22, 2020
 *      Author: Ghanem
 */

#ifndef ECUAL_LCD_LCD_H_
#define ECUAL_LCD_LCD_H_
/************************************************************************/
/*		                        Includes                 		        */
/************************************************************************/
#include "std_types.h"

/************************************************************************/
/*		                        Defines                 		        */
/************************************************************************/
/* LCD Commands definition */
#define LCD_FOUR_BITS			(0x33)
#define LCD_FOUR_BITS_CONFIRM	(0x32)
#define TWO_LINES_5x7_DOTS  	(0x28)
#define LCD_CLEAR           	(0x01)
#define DISPLAY_ON          	(0x0C)
#define CURSOR_ON           	(0x02)
#define INCREMENT_CURSOR    	(0x06)

/* cursor position to DDRAM mapping */
#define DDRAM_OFFSET	       	(0x80)	//DB7: set DD RAM address

#define ROW0_OFFSET				(0x00)
#define ROW1_OFFSET				(0x40)
#define ROW2_OFFSET				(0x10)
#define ROW3_OFFSET				(0x50)

#define ROW0					(0U)
#define ROW1					(1U)
#define ROW2					(2U)
#define ROW3					(3U)

/************************************************************************/
/*		            External Functions' Prototypes      		        */
/************************************************************************/
/*
 * Function:LCD_init.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function is initiate the LCD pins to be output either data or control pins. and start send
 * sequence of commands depending on the needed configuration i.e {8-bits or 4 bits, dots matrix 5*7 or 5*10}
 * there are two modes of operation:
 * 1.OS system: at each time LCD_init() called one of the commands would be
 * send until all of commands are ended then the task would be deleted.
 * 2.Super Loop: at this case the delays are allowed so commands could be send after each other directly.
 */
ERR_STATUS LCD_init(void);

/*
 * Function:LCD_sendCommand.
 *
 * Inputs: u8_command -> represent the required command to be send to LCD.
 *
 * Return: 	ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function send the required command to LCD i.e (CLEAR, go to certain position...)
 * for each system mode function has a different operation.LCD need processing delay between
 * the upper nibble and lower one so we divide the function to two parts
 * for OS mode -> call command function two times to send the whole command.
 * 2.Super Loop: at this case we just add delay between the first nibble and lower one.
 */
ERR_STATUS LCD_sendCommand(uint8 u8_command);

/*
 * Function:LCD_displayCharacter.
 *
 * Inputs: u8_data -> represent the data to be displayed on LCD.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function send the required data by dividing it on two nibbles.
 */
ERR_STATUS LCD_displayCharacter(uint8 u8_data);

/*
 * Function:LCD_displayString.
 *
 * Inputs: au8_string -> array of characters to be displayed on LCD.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function has two approach:
 * OS mode	-> at each time function called just send one byte until string is end then delete the task
 * Super loop -> block on the string until all characters is sent.
 */
ERR_STATUS LCD_displayString(uint8 au8_string[]);

/*
 * Function:LCD_gotoRowColumn.
 *
 * Inputs: u8_rows -> the line to move the cursor on it (ROW_0, ROW_1).
 *		   u8_column -> represent the column at the selected row.
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function move the cursor to the required position.
 * OS mode	-> just need to be called twice to send the command successfully
 * Super loop -> delay on command function would handle it on one time.
 */
ERR_STATUS LCD_gotoRowColumn(uint8 u8_row,uint8 u8_column);
/*
 * Function:LCD_sendString_RowCol.
 *
 * Inputs: au8_string -> array of characters to be displayed on LCD.
 *		   u8_rows -> the line to move the cursor on it (ROW_0, ROW_1).
 *		   u8_column -> represent the column at the selected row.
 *
 * Return: ERROR_Status each error code represent certain error to ease debugging
 *
 * Description: function has two approach:
 * OS mode	-> call  LCD_gotoRowColumn() function until sending command is finished,
 *  then just send one byte each time using LCD_displayString() function until string is end then delete the task
 *
 * Super loop -> delay on sending command would handle LCD_gotoRowColumn() function,
 * then block on the string until all characters is sent.
 */
ERR_STATUS LCD_sendString_RowCol(uint8 u8_row,uint8 u8_column,uint8 au8_string[]);




#endif /* ECUAL_LCD_LCD_H_ */
