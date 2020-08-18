/************************************************************************
 * i2c.c																																*
 * Grubmüller Stefan, Marx Clemens																			*
 * May 2020																															*
 * Library for I2C																											*
************************************************************************/


#ifndef __I2CLIB_H__
#define __I2CLIB_H__

#ifdef __I2CLIB_MOD__
#define EXPORT
#else
#define EXPORT extern
#endif

//	Includes
#include "stm32f10x.h"                  // Device header

//	Defines
#define I2C_READ							1					// Use for i2c_call->RoW to read from bus
#define I2C_WRITE							0					// Use for i2c_call->RoW to write to bus

#define I2C_SEND_ACK					1					// Use for i2c_read_byte->ack to write an ACK after Byte is received
#define I2C_SEND_NACK					0         // Use for i2c_read_byte->ack to write an NACK and stop communication after Byte is received

#define END_WITH_STOP					1         // Use for i2c_write->endwithstop to stop communtication after buffer is tranmited
#define END_WITHOUT_STOP			0         // Use for i2c_write->endwithstop to do not stop communtication after buffer is tranmited

// 	Stuctures

typedef struct													// Define device with options, you can use more than one device
{
	char Adress;                          // Slave Adress
	I2C_TypeDef * I2C_Pereph;             // I2C Perepherie (I2C1 or I2C2)
	long pclr;                            // Perepherial Clock Rate (APB1) [1MHz]
	long sclr;                            // SCL Clock Rate (I²C) [1kHz]
	long maxRiseTime;                     // maximal Rise Time of SCL (default: 1000) [1ns]
}I2C_Device;

typedef struct													// Define SCL and SDA for I2C communication
{
	GPIO_TypeDef * 	GPIOx;								// GPIO Perepherie (e.g. GPIOA)
	int 						Pin;									// GPIO Pin number
}I2C_PIN_CONF;


// Function:			i2c_init(*device, *SCL, *SDA)
// Description:		Perepheral initialisation for I²C device
// Parameters:		device:				The Device to init
//								SCL:					The Portpin for SCL
//								SDA:					The Portpin for SDA
EXPORT void i2c_init(I2C_Device * device, I2C_PIN_CONF * SCL, I2C_PIN_CONF * SDA);

// 
// Function:			i2c_write_byte(*device, byte)
// Description:		write one Byte to the I²C-Bus
// Parameters:		device:				The Divice to communicate
//								byte:					The Byte to transmit
EXPORT void i2c_write_byte(I2C_Device * device, char byte);

// 
// Function:			i2c_write(*device, *buffer, size, endwithstop)
// Description:		Write a more Bytes to teh I²C-Bus
// Parameters:		device:				The Divice to communicate
//								buffer:				The Byte-Array filled with data
//								size:					The size of the Byte-Array
//								endwithstop:	END_WITH_STOP:		communication ends after last byte
//															END_WITHOUT_STOP:	communication does not end
EXPORT void i2c_write(I2C_Device * device, char * buffer, int size, int endwithstop);

// 
// Function:			i2c_read_byte(*device, ack)
// Description:		read one Byte from I²C-Bus
// Parameters:		device:				The Divice to communicate
//								ack:					I2C_SEND_ACK:			send an ACK
//															I2C_SEND_NACK:		sand a NACK and stop communication
//								return value:	The received byte
EXPORT char i2c_read_byte(I2C_Device * device, char ack);

// 
// Function:			i2c_read(*device, *buffer, size)
// Description:		Read multiple Bytes from I²C-Bus
// Parameters:		device:				The Divice to communicate
//								buffer:				The buffer to safe the received data
//								size:					The size of the buffer
EXPORT void i2c_read(I2C_Device * device, char * buffer, int size);

// 
// Function:			i2c_call(*device, RoW)
// Description:		Start an I²C communication to read or write
// Parameters:		device:				The Divice to communicate
//								RoW:					I2C_READ:					Start communication in Receiving Mode
//															I2C_WRITE:				Start communication in Transmiting Mode
EXPORT void i2c_call(I2C_Device * device, char RoW);

// 
// Function:			i2c_startbit(*device)
// Description:		Send the Start Condition
// Parameters:		device:				The Divice to communicate
EXPORT void i2c_startbit(I2C_Device * device);

// 
// Function:			i2c_stopbit(*device)
// Description:		Send the Stop Conditon
// Parameters:		device:				The Divice to communicate
EXPORT void i2c_stopbit(I2C_Device * device);

#endif
