/*
	HardwareSpiDriver.h

	Copyright 2011 Sebastien Besombes <sebastien.besombes@gmail.com>

 	This file is part of the ShockBurst arduino library.

	Shockburst is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	ShockBurst is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

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
