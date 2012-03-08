#ifndef _SPI_DRIVER_
#define _SPI_DRIVER_

extern "C"
{
	#include <string.h>
	#include <inttypes.h>
}

#include <SPI.h>

class SpiDriver
{
public:
	virtual uint8_t transfer(uint8_t data) ;
	virtual uint8_t transfer(uint8_t* data, uint8_t size) ;
	virtual uint8_t read(uint8_t* data, uint8_t size) ;

	virtual void begin() ;
	virtual void end() ;
} ;

#endif
