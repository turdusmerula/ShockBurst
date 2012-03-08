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
	nrf24l.mac = {0x00, 0x1F, 0x1F, 0x1F, 0xFF} ;
	nrf24l.mac_size = AW_5B ;

	nrf24l.config() ;

	//Enable pipe 0 for ACK
	uint8_t mac[5]={0x00, 0x2F, 0x2F, 0x2F, 0xFF} ;
	nrf24l.setPipe(0, mac) ;

	attachInterrupt(0, nrf24l_irq, FALLING) ;  // interrupt 0 digital pin 2

	Serial.println("Init done.") ;

	nrf24l.flushRx() ;
	nrf24l.flushTx() ;
}

void loop()
{
	uint8_t data[32] ;
	int len ;

	energy.Idle() ;

	while(nrf24l.state.dataReceived==1)
	{
		len = nrf24l.getData(data) ;
		data[len] = 0 ;
		Serial.print("RX pipe ") ;
		Serial.print((int)nrf24l.state.pipe) ;
		Serial.print("/") ;
		Serial.print(len) ;
		Serial.print(":") ;
		Serial.println((char*)data) ;
	}
}

void nrf24l_irq()
{
	nrf24l.nrf24l_irq() ;
}
