#include "HardwareSpiDriver.h"

uint8_t HardwareSpiDriver::transfer(uint8_t data)
{
	return SPI.transfer(data) ;
}

uint8_t HardwareSpiDriver::transfer(uint8_t* data, uint8_t size)
{
	for(uint8_t i=0 ; i<size ; i++)
		SPI.transfer(data[i]) ;
	return size ;
}

uint8_t HardwareSpiDriver::read(uint8_t* data, uint8_t size)
{
	for(uint8_t i=0 ; i<size ; i++)
		data[i] = SPI.transfer(data[i]) ;
	return size ;
}

void HardwareSpiDriver::begin()
{
	SPI.begin() ;
	SPI.setDataMode(SPI_MODE0) ;
	SPI.setClockDivider(SPI_2XCLOCK_MASK) ;
}

void HardwareSpiDriver::end()
{
}
