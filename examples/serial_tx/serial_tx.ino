// Do not remove the include below
#include <SPI.h>
#include <ShockBurst.h>
#include <nRF24L01.h>
#include <Enerlib.h>

ShockBurst nrf24l ;
Energy energy ;

void nrf24l_irq() ;

void setup()
{
	Serial.begin(57600*2) ;

	nrf24l.init() ;

	nrf24l.channel = 60 ;
	nrf24l.mac = {0x00, 0x2F, 0x2F, 0x2F, 0xFF} ;
	nrf24l.mac_size = AW_5B ;

	nrf24l.config() ;

	//Enable pipe 0 for ACK
	uint8_t mac[5]={0x00, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(0, mac) ;
	mac = {0x01, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(1, mac) ;
	mac = {0x02, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(2, mac) ;
	mac = {0x03, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(3, mac) ;
	mac = {0x04, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(4, mac) ;
	mac = {0x05, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.setPipe(5, mac) ;

	//Attach interrupt
	attachInterrupt(0, nrf24l_irq, FALLING) ;  // interrupt 0 digital pin 2

	Serial.println("Init done.") ;

	nrf24l.flushRx() ;
	nrf24l.flushTx() ;

}

uint8_t cpt=0 ;
uint8_t data[33] ;
void loop()
{
	cpt = 0 ;
	data[0] = 0 ;
	while(true)
	{
		// if we get a valid byte, read analog ins:
		if(Serial.available()>0)
		{
		    // get incoming byte:
			uint8_t in=Serial.read() ;

		    if(in=='\r')
		    {
		    	Serial.println("") ;
		    	break ;
		    }
		    else
		    {
		    	Serial.print(in) ;
		    	data[cpt] = in ;
		    	data[cpt+1] = 0 ;
		    	cpt++ ;
		    }
		}

		if(cpt>31)
	    {
	    	Serial.println("") ;
	    	break ;
	    }
	}

	Serial.print("TX: ") ;
	Serial.println((char*)data) ;


	if(nrf24l.txFifoFull()==true)
	{
		Serial.println("full") ;
		return ;
	}

	nrf24l.send(data, cpt) ;
	energy.Idle() ;

	if(nrf24l.state.transmitOk==0)
		Serial.println("Packet not transmitted") ;
	else
		Serial.println("Packet ok") ;

}

void nrf24l_irq()
{
	nrf24l.nrf24l_irq() ;
}
