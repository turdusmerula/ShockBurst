// Do not remove the include below
#include <SPI.h>
#include <ShockBurst.h>
#include <nRF24L01.h>
#include <Enerlib.h>
#include <FlexiTimer2.h>

ShockBurst nrf24l ;
Energy energy ;

#define TIMEOUT 500

bool interrupt=false ;
bool tout=false ;
uint32_t cpt=0 ;
uint32_t err=0 ;
uint32_t n_nrf24l_irq=0 ;
uint32_t n_timeout=0 ;

void nrf24l_irq()
{
	interrupt = true ;
	nrf24l.nrf24l_irq() ;
	n_nrf24l_irq++ ;
}

void timeout()
{
	Serial.println("TX timeout") ;
	tout = true ;
	n_timeout++ ;
}


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

	nrf24l.flush() ;

	//Initialize timeout timer
	FlexiTimer2::set(500, timeout) ;
	FlexiTimer2::start() ;

	//Attach interrupt
	attachInterrupt(0, nrf24l_irq, FALLING) ;  // interrupt 0 digital pin 2

	Serial.println("TX Init done.") ;
}

void loop()
{
	while(true)
	{
		//Reset timer
		FlexiTimer2::start() ;

		tout = false ;
		nrf24l.clearState() ;
		nrf24l.send((uint8_t*)&cpt, sizeof(uint32_t)) ;
		cpt++ ;

		//Reset timer
		FlexiTimer2::start() ;

		while(nrf24l.state.transmitErr==0 && nrf24l.state.transmitOk==0 && tout==false)
		{
			if(Serial.available()>0)
			{
				Serial.read() ;

				Serial.print("Err ") ;
				Serial.println(err) ;
				Serial.print("cpt ") ;
				Serial.println(cpt) ;
				Serial.print("n_nrf24l_irq ") ;
				Serial.println(n_nrf24l_irq) ;
				Serial.print("n_timeout ") ;
				Serial.println(n_timeout) ;
			}
		}

		if(nrf24l.state.transmitErr==1 || tout==true)
		{
			err++ ;
			Serial.println("-------------") ;
			Serial.print("Err ") ;
			Serial.println(err) ;
			Serial.print("cpt ") ;
			Serial.println(cpt) ;
			Serial.print("losts ") ;
			Serial.println((uint32_t)nrf24l.state.losts) ;
			Serial.print("trys ") ;
			Serial.println((uint32_t)nrf24l.state.trys) ;

			Serial.print("n_nrf24l_irq ") ;
			Serial.println(n_nrf24l_irq) ;
			Serial.print("n_timeout ") ;
			Serial.println(n_timeout) ;
		}

		n_nrf24l_irq++ ;

		//Reset timer
		tout = false ;
		FlexiTimer2::stop() ;
	}
}

