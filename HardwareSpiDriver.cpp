/*
	HardwareSpiDriver.cpp

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
