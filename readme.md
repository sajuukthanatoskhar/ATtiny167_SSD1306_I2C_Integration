***SSD1306 Attiny167 I2C Integration***

This repository contains the source needed to display characters to a SSD1306 screen

There are plenty of Arduino based solutions and other platforms that use the SSD1306, but the ATTiny did not as far as I could recall so I made this.  

You are free to distribute or change this as per the GPL.  

**Compatibility** 

It has only been tested on 

ATtiny167

SSD1306 128x64 pixel screen

**General Use**

1.  Connect Attiny167 up to SSD1306 Module via I2C (Pin 19/20 were used in a prior project)
2.  Initialise the I2C using i2c.c:i2c_init()
3.  Initialise the OLED Display using OLED_init() in lcd.c
4.  Set the cursor position (invisible however) with OLED SetCursor(0,0)
5.  OLED_DisplayString for a small font (one line), or the OLED_Display12String for something bigger (takes two lines) e.g OLED_DisplayString("Hello World"), this should read from the zeichensatz and print out your characters



