#ifndef _lcd_c_
#define _lcd_c_
#include "lcd.h"
#include "avr/pgmspace.h"


/*------------------------------------------------------------------------------------------*\
The LCD screen in question here is the SSD1306 
https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
http://www.solomon-systech.com/en/product/advanced-display/oled-display-driver-ic/ssd1306/

This only uses the 128x64.  There is another with a different size but this code is not compatible with that.  
\*------------------------------------------------------------------------------------------*/
static uint8_t OLED_WriteOffset = 0;

/*------------------------------------------------------------------------------------------*\
Initialises the LCD
Does a certain number of things 
\*------------------------------------------------------------------------------------------*/
PROGMEM	const uint8_t p_init_screen[] = {
	(SSD1306_ADDRESS<<1),
	SSD1306_COMMAND, 
	SSD1306_DISPLAY_OFF, 
	SSD1306_SET_DISPLAY_CLOCK_DIV_RATIO,
	0x80,
	SSD1306_SET_MULTIPLEX_RATIO, 
	0x3F,
	SSD1306_SET_DISPLAY_OFFSET, 
	0x0, 
	(SSD1306_SET_START_LINE | 0x0), 
	SSD1306_CHARGE_PUMP, 
	0x14, 
	SSD1306_MEMORY_ADDR_MODE, 
	0x00, 
	(SSD1306_SET_SEGMENT_REMAP | 0x1), 
	SSD1306_COM_SCAN_DIR_DEC, 
	SSD1306_SET_COM_PINS, 
	0x12, 
	SSD1306_SET_CONTRAST_CONTROL, 
	0xCF, 
	SSD1306_SET_PRECHARGE_PERIOD, 
	0xF1, 
	SSD1306_SET_VCOM_DESELECT, 
	0x40, 
	SSD1306_DISPLAY_ALL_ON_RESUME, 
	SSD1306_NORMAL_DISPLAY, 
	SSD1306_DISPLAY_ON
};

/*
Resets the position of the cursor to the top left of the OLED screen, setting the maximum boundary to the bottom right
*/
PROGMEM	const uint8_t p_reset_pos[] = {
	(SSD1306_ADDRESS<<1),
	SSD1306_COMMAND,
	SSD1306_SET_COLUMN_ADDR,
	0,
	0x7F,
	SSD1306_SET_PAGE_ADDR,
	0,
	7
};

PROGMEM	const uint8_t p_clear_byte_cmd[] = {
	(SSD1306_ADDRESS<<1),
	SSD1306_DATA,
	0x00
};

PROGMEM	const uint8_t p_set_cursor_cmd[] = {
	(SSD1306_ADDRESS<<1),
	SSD1306_COMMAND,
	SSD1306_SET_COLUMN_ADDR,
	0x00, /* linenumber, p_set_cursor_cmd[4] */
	0x7C, // End Column Addr -3, because 
	SSD1306_SET_PAGE_ADDR,
	0x00, /* cursorposition,  p_set_cursor_cmd[6] */
	0x07  // End Page addr
};

uint8_t lcd_transfer_buffer[68];

/*------------------------------------------------------------------------------------------*\
Initialises the OLED using the array
Input:
	Nothing
Output:	
	Nothing
\*------------------------------------------------------------------------------------------*/
void OLED_Init(void) {
	memcpy_P(lcd_transfer_buffer, p_init_screen, sizeof(p_init_screen));
	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, sizeof(p_init_screen));
	OLED_Clear();
}

/*------------------------------------------------------------------------------------------*\
Allocates the appropriate I2C bytes necessary to send data to the LCD screen.  
Input:
	Nothing
Output:	
	Returns the byte pointer with the necessary header bytes to allow the writing of bytes to the screen
\*------------------------------------------------------------------------------------------*/
uint8_t *OLED_AllocDisplayString(void) {
	lcd_transfer_buffer[0] = SSD1306_ADDRESS << 1;
	lcd_transfer_buffer[1] = SSD1306_DATA_CONTINUE;
	OLED_WriteOffset = 2;
	return &lcd_transfer_buffer[OLED_WriteOffset];
}

/*------------------------------------------------------------------------------------------*\
Retrieves the string from PROG memory and prints it
Input:
	str: Input str array located in memory 
\*------------------------------------------------------------------------------------------*/
uint8_t _OLED_DisplayPgmString(uint16_t str) {
	uint8_t len = 0;
	uint8_t ch;
	uint16_t start = str;
	uint8_t *output_buffer  = OLED_AllocDisplayString(); // Set up the buffer pointer to ensure 
	
	while((ch = pgm_read_byte(str)) != 0) {  /* input is a NULL taminated string */
		uint8_t line;
		uint8_t * PROGMEM index;  /* pointer to program memory */
		/* check whether the value of th character is 
		 * higher then the size of the lookup table    */
		if(ch >= (int8_t)(sizeof(font) / sizeof(font[0]))) {  
			index = pgm_read_ptr(&font[0]);  /* use default character */
		} else {
			index = pgm_read_ptr(&font[ch]);  /* get the font pointer */

			/* if character is unavalible, use default char */
			if(index == NULL) 
				index = pgm_read_ptr(&font[0]);
		}

		/* read all 5 bitmap values from program memory */
		for(line = 0 ; line < ZEICHENSATZ_SIZE ; line++) {
			output_buffer [len++] = pgm_read_byte(index + line);
		}
		output_buffer [len++] = 0x00; /* add space between to chars */
		str++;

		if(len >= 64) {
			USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
			output_buffer  = OLED_AllocDisplayString();
			len = 0;
		}
	}
	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
	return str - start;
}

/*------------------------------------------------------------------------------------------*\
Retrieves the string from PROG memory and prints it
Input:
	page: the line of the OLED display (0 - 7)
	column: the starting column (0 - 127)
	str: the string array located in PROG memory
Output:
	returns the length of the character string from program memory
\*------------------------------------------------------------------------------------------*/
uint8_t _OLED_Display12PgmString(uint8_t page, uint8_t column, uint16_t str) {
	uint8_t len;
	uint8_t ch;
	uint16_t start = str;
	uint8_t offset;

	for(offset = 0 ;  offset < 2 ; offset += 1) {
		uint8_t *output_buffer;
		str = start;
		OLED_SetCursor(page + offset, column);
		output_buffer  = OLED_AllocDisplayString();
		len = 0;

		while((ch = pgm_read_byte(str)) != 0) {  /* input is a NULL taminated string */
			uint8_t line;
			uint8_t * PROGMEM index;  /* pointer to program memory */
			/* check whether the value of th character is 
			 * higher then the size of the lookup table    */
			if(ch >= (int8_t)(sizeof(font12) / sizeof(font12[0]))) {  
				index = pgm_read_ptr(&font12[0]);  /* use default character */
			} else {
				index = pgm_read_ptr(&font12[ch]);  /* get the font pointer */

				/* if character is unavalible, use default char */
				if(index == NULL) 
					index = pgm_read_ptr(&font12[0]);
			}

			/* read all 12 bitmap values from program memory */
			for(line = 0 ; line < ZEICHENSATZ_SIZE12 ; line++) {
				output_buffer [len++] = pgm_read_byte(index + line + (offset * ZEICHENSATZ_SIZE12));
			}
			output_buffer [len++] = 0x00; /* add space between to chars */
			output_buffer [len++] = 0x00; /* add space between to chars */
			str++;
			if(len >= 64) {
				USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
				output_buffer  = OLED_AllocDisplayString();
				len = 0;
			}
		}
		USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
	}

	return str - start;
}



/*------------------------------------------------------------------------------------------

This prints the 12px Font, refer to zeichensatz12.h for font details
There are two parts to this function, the first part will print the 'higher' or the top byte of the two bytes that make up this font.  The second part is then the lower.  
inputs are the page number of the OLED, the column (0 -> 127), and the string
Input:
	page: the line of the OLED display (0 - 7)
	column: the starting column (0 - 127)
	str: the character string array to be printed
Output:
	returns a byte
*\
\*------------------------------------------------------------------------------------------*/
void OLED_Display12String(uint8_t page, uint8_t column, char *str) {
	uint8_t len = 0;
	uint8_t *output_buffer;
	char *start = str;
	/*
	The OLED_SetCursor should be run before len = 0;  
	It seemed to cause problems, if it doesn't seem it would, it needs to be unit tested first before making other changes to this function
	*/
	OLED_SetCursor(page, column);	/* Print upper part of font */
	output_buffer  = OLED_AllocDisplayString();

	while(*str) {  /* input is a NULL taminated string */
		uint8_t line;
		uint8_t * PROGMEM index;  /* pointer to program memory */
		/* check whether the value of th character is 
		 * higher then the size of the lookup table    */
		if(*str >= (int8_t)(sizeof(font12) / sizeof(font12[0]))) {  
			index = pgm_read_ptr(&font12_default[0]);  /* use default character */
		} else {
			index = pgm_read_ptr(&font12[(uint8_t)*str]);  /* get the font pointer */
			/* if character is unavalible, use default char */
			if(index == NULL) {			
				index = pgm_read_ptr(&font[0]);
			}
		}
		
		//Read the first line, then the second size	
		/* read all 12 upper bitmap values from program memory */
		for(line = 0 ; line < ZEICHENSATZ_SIZE12 ; line++) {
			output_buffer [len++] = pgm_read_byte(index + line);
		}
		output_buffer [len++] = 0x00; /* add space between to chars */
		output_buffer [len++] = 0x00; /* add space between to chars */
		str++;

		if(len >= 64) {
			USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
			output_buffer  = OLED_AllocDisplayString();
			len = 0;
		}
	}

		
	str = start;
		
	/* physical print upper half of font */
	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
	len = 0;


	OLED_SetCursor(page + 1, column);	/* print lower half of font */
	output_buffer  = OLED_AllocDisplayString();

	while(*str) {  /* input is a NULL taminated string */
		uint8_t line;
		uint8_t * PROGMEM index;  /* pointer to program memory */
		
		/* check whether the value of th character is 
		 * higher then the size of the lookup table    */
		if(*str >= (int8_t)(sizeof(font12) / sizeof(font12[0]))) {  
			index = pgm_read_ptr(&font12_default[0]);  /* use default character */
		} else {
			index = pgm_read_ptr(&font12[(uint8_t)*str]);  /* get the font pointer */
			/* if character is unavalible, use default char */
			if(index == NULL) {			
				index = pgm_read_ptr(&font[0]);
			}
		}
		
		//Read the first line, then the second size	
		/* read all 12 upper bitmap values from program memory */
		for(line = 0 ; line < ZEICHENSATZ_SIZE12 ; line++) {
			output_buffer [len++] = pgm_read_byte(index + ZEICHENSATZ_SIZE12 + line);
		}
		output_buffer [len++] = 0x00; /* add space between to chars */
		output_buffer [len++] = 0x00; /* add space between to chars */

		str++;

		if(len >= 64) {
			USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
			output_buffer  = OLED_AllocDisplayString();
			len = 0;
		}
	}

	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
}



/*------------------------------------------------------------------------------------------*\
Prints out a string that is 5 pixels in length.  Automatic padding is done for each character

Input is just a string, no maximum length (although anything past 1024/(5+1) might have some unintended consequences
Warning: There is no limit to the number of strings printed and you run the risk of overflowing the screen

Input:
	str: String pointer
Output:
	Nothing
\*------------------------------------------------------------------------------------------*/
void OLED_DisplayString(char *str) {
	uint8_t len = 0;

	uint8_t *output_buffer  = OLED_AllocDisplayString();

	while(*str) {  /* input is a NULL taminated string */
		uint8_t line;

		uint8_t * PROGMEM index;  /* pointer to program memory */
		/* check whether the value of th character is 
		 * higher then the size of the lookup table    */
		if(*str >= (int8_t)(sizeof(font) / sizeof(font[0]))) {  
			index = pgm_read_ptr(&font[0]);  /* use default character */
		} else {
			index = pgm_read_ptr(&font[(uint8_t)*str]);  /* get the font pointer */

			/* if character is unavalible, use default char */
			if(index == NULL) 
				index = pgm_read_ptr(&font[0]);
		}

		/* read all 5 bitmap values from program memory */
		for(line = 0 ; line < ZEICHENSATZ_SIZE ; line++) {
			output_buffer [len++] = pgm_read_byte(index + line);
		}
		output_buffer [len++] = 0x00; /* add space between to chars */
		str++;
	}

	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, OLED_WriteOffset + len);
}

/*------------------------------------------------------------------------------------------*\
Clears the entire LCD screen

Input:
	Nothing
Output:
	Nothing
\*------------------------------------------------------------------------------------------*/
void OLED_Clear(void) {
	/*
	   Reset to top left of LCD
	 */
	memcpy_P(lcd_transfer_buffer, p_reset_pos, sizeof(p_reset_pos));
	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, sizeof(p_reset_pos));

	memcpy_P(lcd_transfer_buffer, p_clear_byte_cmd, sizeof(p_clear_byte_cmd));

	for (uint16_t i = 0; i < 1024; i++) {
		USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, sizeof(p_clear_byte_cmd));
	}

	memcpy_P(lcd_transfer_buffer, p_reset_pos, sizeof(p_reset_pos));
	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, sizeof(p_reset_pos));

}

/*------------------------------------------------------------------------------------------*\
 * This command does two things on the LCD
 * It sets the page number (line number) - the range is from 0x00 to 0x07 for the screen we use.  
 * Bigger screen go up to 0x0D
 * It then sets the column number (cursorposition), and can be set from 0x00 to 0x7F.  
 *
 * Mit diesem 4 Nummern (linenumber, 0x7F, cursorposition, 0x07) kann man die Cursor einstellen, 
 * wo mann will.   i

 Input: 
 	cursorposition: the column number for the cursor
	linenumber: the line number for the cursor
 Output: 
 	Nothing
\*------------------------------------------------------------------------------------------*/
void OLED_SetCursor(uint8_t cursorposition, uint8_t linenumber) {

	memcpy_P(lcd_transfer_buffer, p_set_cursor_cmd, sizeof(p_set_cursor_cmd));
	lcd_transfer_buffer[3] = linenumber;
	lcd_transfer_buffer[6] = cursorposition;

	USI_I2C_Master_Start_Transmission(lcd_transfer_buffer, sizeof(p_set_cursor_cmd));
}


/*
This function clears part of the screen in a rectangular area 

*/
void OLED_Clear_Rec_Section_LCD(uint8_t start_column, uint8_t end_column, uint8_t start_row, uint8_t end_row) {
	
	//check that the LCD screen is in the correct addressing mode
	//This assumes that the addressing mode has not been changed from startup default (Row 0, Column 0 to Column 127 then Row 1, Column 0 to Column 127, and so on)
	//Write " "'s to this area
	//Done
}
#endif
