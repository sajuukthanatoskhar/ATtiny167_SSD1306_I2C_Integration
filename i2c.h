#ifndef _i2c_h_
#define _i2c_h_


#include <avr/io.h>
#include <util/delay.h>

//I2C Bus Specification v2.1 FAST mode timing limits
#ifdef I2C_FAST_MODE	
	#define I2C_TLOW	1.3
	#define I2C_THIGH	0.6

//I2C Bus Specification v2.1 STANDARD mode timing limits
#else
	#define I2C_TLOW	4.7
	#define I2C_THIGH	4.0
#endif

//Microcontroller Dependent Definitions
#if defined (__AVR_ATtiny24__) | \
	defined (__AVR_ATtiny44__) | \
	defined (__AVR_ATtiny84__)
	#define DDR_USI			DDRA
	#define PORT_USI		PORTA
	#define PIN_USI			PINA
	#define PORT_USI_SDA	PA6
	#define PORT_USI_SCL	PA4
	#define PIN_USI_SDA		PINA6
	#define PIN_USI_SCL		PINA4
#endif

#if defined(__AVR_AT90Tiny2313__) | \
	defined(__AVR_ATtiny2313__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PB5
    #define PORT_USI_SCL        PB7
    #define PIN_USI_SDA         PINB5
    #define PIN_USI_SCL         PINB7
#endif

#if defined(__AVR_ATtiny167__) 
	    #define DDR_USI             DDRB
	    #define PORT_USI            PORTB
	    #define PIN_USI             PINB
	    #define PORT_USI_SDA        PB0
	    #define PORT_USI_SCL        PB2
	    #define PIN_USI_SDA         PINB0
	    #define PIN_USI_SCL         PINB2
#endif

#if defined(__AVR_ATtiny167__) && defined (USIPOS_Val)
#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PA4
#define PORT_USI_SCL        PA5
#define PIN_USI_SDA         PINA4
#define PIN_USI_SCL         PINA5
#endif

#define I2C_WRITE 0
#define I2C_READ 1

// USI I2C Master Transceiver Start
// Starts the transceiver to send or receive data based on the r/w bit
char USI_I2C_Master_Transceiver_Start(char *msg, char msg_size);
extern void set_SCL_high();
extern void init_i2c();

#endif /*--- #ifndef _i2c_h_ ---*/
