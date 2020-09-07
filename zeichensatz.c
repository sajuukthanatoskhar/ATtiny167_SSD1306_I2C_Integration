#ifndef _zeichensatz_c_
#define _zeichensatz_c_

/*------------------------------------------------------------------------------------------*\

  Converts the character to a uint8_t pointer that allows for the OLED display to later display it as a series of pixels
  This function is only for the 12 pixel font.  It has a 4 px kerning at the end of each character.  

  Input:
   	*outbuffer: output byte array to be put in to a form to facilitate the display of characters as a font.  This is developed to print characters to the LCD screen.  
	*inbuffer: This is the character array being translated to the outbuffer, each value here should be an ASCII compatible A-Z/0-9/special character. It is only read.  
	line: This the line of the character (it goes from 0 to 11, where 0-11 is the actual character, 12-15 is the padding)
	count: this is for limiting the reference to parts of the actual font being printed
	idx: 
  Output returns the value of idx but also changes the value of the outbuffer, original intention of the function


  Refer to the 5 px font version of this function.  One of the core differences is that the font being used is split from 2 bytes due to the size of the font (2 bytes high(deep) by 2 bytes long)
\*------------------------------------------------------------------------------------------*/
uint8_t zeichensatz12_char_to_pixel(uint8_t *outbuffer, uint8_t *inbuffer, uint8_t line, uint8_t count) {
	uint8_t idx;

	switch(line) {
		case 12:
		case 13:
		case 14:
		case 15:
			for(idx = 0 ; idx < count ; idx++) {
				outbuffer[(idx << 1)]     =  0;
				outbuffer[(idx << 1) + 1] =  0;
			}
			break;

		default:
			for(idx = 0 ; idx < count ; idx++) {
				uint16_t * PROGMEM index; // The pointer reference used to point to specific characters from zeichensat12.h
				uint16_t ch; // holds the 16 bit wide variable to be split across two lines on the LCD
				/*
				If not a uint8_t sized variable, then index is a NULL, not a '0' (zero), character! 
				*/
				if(inbuffer[idx] >= (sizeof(font12) / sizeof(font12[0]))) {
					index = pgm_read_ptr(&font[0]);
				} else {
					index = pgm_read_ptr(&font12[inbuffer[idx]]);
					if(index == NULL)
						index = pgm_read_ptr(&font12[0]);
				}
				ch = pgm_read_word(index + line);
				outbuffer[(idx << 1)]     =  (uint8_t)ch;
				outbuffer[(idx << 1) + 1] =  (uint8_t)(ch >> 8);
			}
	}
	return count << 1;
}


/*------------------------------------------------------------------------------------------*\

  Converts the character to a uint8_t pointer that allows for the OLED display to later display it as a series of pixels
  This function is only for the 5(6) pixel font.  It has a 2 px kerning at the end of each character.  

  Input:
   	*outbuffer: output byte array to be put in to a form to facilitate the display of characters as a font.  This is developed to print characters to the LCD screen.  
	*inbuffer: This is the character array being translated to the outbuffer, each value here should be an ASCII compatible A-Z/0-9/special character. It is only read.  
	line: This the line of the character (it goes from 0 to 7, where 0-5 is the actual character, 6-7 is the padding)
	count: this is for limiting the reference to parts of the actual font being printed
	idx: 
  Output returns the value of idx but also changes the value of the outbuffer, original intention of the function

\*------------------------------------------------------------------------------------------*/
uint8_t zeichensatz_char_to_pixel(uint8_t *outbuffer, uint8_t *inbuffer, uint8_t line, uint8_t count) {
	uint8_t idx;

	switch(line) {
		case 6:
		case 7:
			for(idx = 0 ; idx < count ; idx++) {
				outbuffer[idx] =  0;
			}
			break;

		default:
			for(idx = 0 ; idx < count ; idx++) {
				uint8_t * PROGMEM index;
				/*
				If the inbuffer is greater than the size of a uint8_t, we set the index referred to the NULL, not '0' (zero), character
				*/
				if(inbuffer[idx] >= (sizeof(font) / sizeof(font[0]))) { 
					index = pgm_read_ptr(&font[0]);
				} else {
					index = pgm_read_ptr(&font[inbuffer[idx]]); // Gets the character from the font/zeichensatz.h table 
					if(index == NULL)
						index = pgm_read_ptr(&font[0]);
				}
				outbuffer[idx] = pgm_read_byte(index + line); // Outputs part of the font to outbuffer
			}
	}
	return idx;
}


#endif /*--- #ifndef _zeichensatz_c_ ---*/
