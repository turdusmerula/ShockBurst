#ifndef _HARDWARE_SPI_DRIVER_
#define _HARDWARE_SPI_DRIVER_

#include <SpiDriver.h>

class HardwareSpiDriver : public SpiDriver
{
public:
	virtual uint8_t transfer(uint8_t data) ;
	virtual uint8_t transfer(uint8_t* data, uint8_t size) ;
	virtual uint8_t read(uint8_t* data, uint8_t size) ;

	virtual void begin() ;
	virtual void end() ;
} ;

#endif
