#include <SPI.h>
#include <ShockBurst.h>
#include <nRF24L01.h>
#include <Enerlib.h>
#include <avr/wdt.h>
#include <FlexiTimer2.h>

ShockBurst nrf24l ;
Energy energy ;

#define TIMEOUT 100
uint32_t n_nrf24l_irq=0 ;
uint32_t n_timeout=0 ;

uint32_t bytes=0 ;
uint32_t packets=0 ;
uint32_t prev=0 ;
uint32_t err=0 ;


void nrf24l_irq()
{
	cli() ;
	nrf24l.nrf24l_irq() ;
	n_nrf24l_irq++ ;
}

void timeout()
{
	//In case an interrupt was lost the RX buffer may go full and cause lot of packets to be lost
	//this code is here to unlock this by clearing the RX fifo
	Serial.println("RX timeout") ;
	nrf24l.flush() ;
	nrf24l.clearState() ;
	n_timeout++ ;
}

void setup()
{
	Serial.begin(57600*2) ;

	nrf24l.init() ;

	nrf24l.channel = 60 ;
	nrf24l.mac = {0x00, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.mac_size = AW_5B ;

	nrf24l.config() ;

	//Enable pipe 0 for ACK
	uint8_t mac[5]={0x00, 0x2F, 0x2F, 0x2F, 0xFF} ;
	nrf24l.setPipe(0, mac) ;

	nrf24l.flush() ;

	//Initialize timeout timer
	FlexiTimer2::set(500, timeout) ;
	FlexiTimer2::start() ;

	//enable watchdog timer at 2s
	Serial.println("Watchdog 8s") ;
	wdt_enable(WDTO_8S);

	//Attach interrupt
	attachInterrupt(0, nrf24l_irq, FALLING) ;  // interrupt 0 digital pin 2

	Serial.println("RX Init done.") ;
}

void loop()
{
	uint32_t val ;
	uint8_t len ;

	while(true)
	{
		wdt_reset() ;

		if(Serial.available()>0)
		{
			Serial.read() ;

			Serial.print("Err ") ;
			Serial.println(err) ;
			Serial.print("bytes ") ;
			Serial.println(bytes) ;
			Serial.print("packets ") ;
			Serial.println(packets) ;
			Serial.print("val ") ;
			Serial.println(val) ;
			Serial.print("n_nrf24l_irq ") ;
			Serial.println(n_nrf24l_irq) ;
			Serial.print("n_timeout ") ;
			Serial.println(n_timeout) ;
		}

		while(nrf24l.state.dataReceived==1)
		{
			//Stop timer, something was received
			FlexiTimer2::stop() ;

			len = nrf24l.getData((uint8_t*)&val) ;
			if(len!=0xFF)
			{
				//Statistics
				bytes += len ;
				packets++ ;
			}
			
			if(val!=prev+1)
				err++ ;

			prev = val ;

			//Reset timer for timeout if something is wrong
			FlexiTimer2::start() ;
		}
	}
}

