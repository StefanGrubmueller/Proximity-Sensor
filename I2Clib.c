//
//	Project:			I2Clib
//	Version:			V0.1a
//	Author:				Jakob Pachtrog | 4BHEL
//	Date:					29.01.2020
//	Description:	Sources for I²C Libary
//

//	Includes
#include "I2C.h"

//	Defines
#define 	__I2CLIB_MOD__

//	Prototypes
void i2c_enable_port(I2C_PIN_CONF * pin);
void i2c_set_pin_mode(I2C_PIN_CONF * pin, char mode);

//	Functions
void i2c_init(I2C_Device * device, I2C_PIN_CONF * SCL, I2C_PIN_CONF * SDA)
{
	i2c_enable_port(SCL);
	i2c_enable_port(SDA);
	i2c_set_pin_mode(SCL, 0xF);
	i2c_set_pin_mode(SDA, 0xF);
	
	// Peripheral input clock configurtion
	if (device->I2C_Pereph == I2C1)
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	else if (device->I2C_Pereph == I2C2)
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		
	device->I2C_Pereph->CR2 &= ~(0x1F << 0x0);
	device->I2C_Pereph->CR2 |= (device->pclr << 0x0);
	
	// I²C Output clock confugurtion
	device->I2C_Pereph->CCR &= ~(0x1 << 15);
	device->I2C_Pereph->CCR &= ~(0x1 << 14);
	device->I2C_Pereph->CCR &= ~0xFFF;
	device->I2C_Pereph->CCR |= (10000/device->sclr/2)/(1000/device->pclr);
	
	// Rise Time configurtion
	device->I2C_Pereph->TRISE = (device->maxRiseTime * device->pclr)/1000 + 1;

	// Additional Configurations
	device->I2C_Pereph->CR1 &= ~(0x1 << 0x1);
	
	// Busy Flag Error - Workaround as ERRATA Sheet
	// 	I2C analog filter may provide wrong value, locking BUSY flag and
	//	preventing master mode entry -- ERRATA Sheet Rev 13 Chapter 2.13.7
	device->I2C_Pereph->CR1 |= I2C_CR1_PE;
	device->I2C_Pereph->CR1 &= ~I2C_CR1_PE;
	i2c_set_pin_mode(SCL, 0x7);
	i2c_set_pin_mode(SDA, 0x7);
	SCL->GPIOx->ODR |= (1 << SCL->Pin);
	SDA->GPIOx->ODR |= (1 << SDA->Pin);
	while (!((SCL->GPIOx->IDR & (1 << SCL->Pin)) && (SDA->GPIOx->IDR & (1 << SDA->Pin))));
	SDA->GPIOx->ODR &= ~(1 << SDA->Pin);
	while (SDA->GPIOx->IDR & (1 << SDA->Pin));
	SCL->GPIOx->ODR &= ~(1 << SCL->Pin);
	while (SCL->GPIOx->IDR & (1 << SCL->Pin));
	SCL->GPIOx->ODR |= (1 << SCL->Pin);
	while (!(SCL->GPIOx->IDR & (1 << SCL->Pin)));
	SDA->GPIOx->ODR |= (1 << SDA->Pin);
	while (!(SDA->GPIOx->IDR & (1 << SDA->Pin)));
	i2c_set_pin_mode(SCL, 0xF);
	i2c_set_pin_mode(SDA, 0xF);
	device->I2C_Pereph->CR1 |= I2C_CR1_SWRST;
	device->I2C_Pereph->CR1 &= ~I2C_CR1_SWRST;
	
	// Peripheral input clock configurtion
	if (device->I2C_Pereph == I2C1)
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	else if (device->I2C_Pereph == I2C2)
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
		
	device->I2C_Pereph->CR2 &= ~(0x1F << 0x0);
	device->I2C_Pereph->CR2 |= (device->pclr << 0x0);
	
	// I²C Output clock confugurtion
	if ((device->sclr <= 100) && (device->pclr >= 2))
	{
		device->I2C_Pereph->CCR &= ~(0x1 << 15);
		device->I2C_Pereph->CCR &= ~(0x1 << 14);
		device->I2C_Pereph->CCR &= ~0xFFF;
		device->I2C_Pereph->CCR |= (1000000/device->sclr/2)/(1000/device->pclr);
	}
	else if ((device->sclr % 10) == 0)
	{
		device->I2C_Pereph->CCR |= (0x1 << 15);
		device->I2C_Pereph->CCR &= ~(0x1 << 14);
		device->I2C_Pereph->CCR &= ~0xFFF;
		device->I2C_Pereph->CCR |= (1000000/device->sclr/3)/(1000/device->pclr);
	}
	
	// Rise Time configurtion
	device->I2C_Pereph->TRISE = (device->maxRiseTime * device->pclr)/1000 + 1;

	// Additional Configurations
	device->I2C_Pereph->CR1 &= ~(0x1 << 0x1);
	
	// Start I2C
	device->I2C_Pereph->CR1 |= I2C_CR1_PE;
}

void i2c_startbit(I2C_Device * device)
{
	device->I2C_Pereph->CR1 |= (0x1 << 0x8);		//(I2C1_CR1_START = 1)
	while(!(device->I2C_Pereph->SR1 & I2C_SR1_SB));
}

void i2c_stopbit(I2C_Device * device)
{
	while((device->I2C_Pereph->SR1 & I2C_SR1_BTF) && (device->I2C_Pereph->SR1 & I2C_SR1_TXE))
	device->I2C_Pereph->CR1 |= (0x1 << 0x9);		//(I2C1_CR1_STOP = 1)
}

void i2c_call(I2C_Device * device, char RoW)
{
	i2c_startbit(device);
	while (!(device->I2C_Pereph->SR1 & I2C_SR1_SB));
	device->I2C_Pereph->DR = (device->Adress << 1) + RoW;
	while (!(device->I2C_Pereph->SR1 & I2C_SR1_ADDR));
	int sr2 = device->I2C_Pereph->SR2;
}

void i2c_write_byte(I2C_Device * device, char byte)
{
	device->I2C_Pereph->DR = byte;
	while (!(device->I2C_Pereph->SR1 & I2C_SR1_TXE));
}

char i2c_read_byte(I2C_Device * device, char ack)
{
	if (ack == 1)
	{
		device->I2C_Pereph->CR1 |= (I2C_CR1_ACK);
		while (!(device->I2C_Pereph->SR1 & I2C_SR1_RXNE));
		return device->I2C_Pereph->DR;
	}
	else if (ack == 0)
	{
		device->I2C_Pereph->CR1 &= ~(I2C_CR1_ACK);
		device->I2C_Pereph->CR1 |= (I2C_CR1_STOP);
		while (!(device->I2C_Pereph->SR1 & I2C_SR1_RXNE));
		return device->I2C_Pereph->DR;
	}
	return 0xFF;
}

void i2c_write(I2C_Device * device, char * buffer, int size, int endwithstop)
{
	i2c_call(device, I2C_WRITE);
	for (int i = 0; i < size; i++)
	{
		i2c_write_byte(device, buffer[i]);
	}
	if (endwithstop == END_WITH_STOP)
	{
		i2c_stopbit(device);
	}
}

void i2c_read(I2C_Device * device, char * buffer, int size)
{
	i2c_call(device, I2C_READ);
	for (int i=0; i < size-1; i++)
	{
		buffer[i] = i2c_read_byte(device, I2C_SEND_ACK);
	}
	buffer[size-1] = i2c_read_byte(device, I2C_SEND_NACK);
}

void i2c_enable_port(I2C_PIN_CONF * pin)
{
	if (pin->GPIOx == GPIOA)
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	else if (pin->GPIOx == GPIOB)
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	else if (pin->GPIOx == GPIOC)
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	else if (pin->GPIOx == GPIOD)
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	else if (pin->GPIOx == GPIOE)
		RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
}

void i2c_set_pin_mode(I2C_PIN_CONF * pin, char mode)
{
	if ((pin->Pin / 8) == 0)
	{
		pin->GPIOx->CRL &= ~(0xF << (pin->Pin%8)*4);
		pin->GPIOx->CRL |= (mode << (pin->Pin%8)*4);
	}
	else
	{
		pin->GPIOx->CRH &= ~(0xF << (pin->Pin%8)*4);
		pin->GPIOx->CRH |= (mode << (pin->Pin%8)*4);
	}
}
