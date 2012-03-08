#include "ShockBurst.h"
#include <Arduino.h>
#include <nRF24L01.h>
#include <HardwareSpiDriver.h>

HardwareSpiDriver HardwareSpi ;

ShockBurst::ShockBurst()
{
	//Default parameters
	cePin = 8 ;
	csnPin = 7 ;
	payload = 32 ;
	channel = 1 ;

	spi = (SpiDriver*)&HardwareSpi ;	//By default use hardware SPI

	//Default parameters
	mac = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7} ;
	mac_size = AW_5B ;
	payload = 0 ;	//dynamic payload by default, no payload length set.

	state.dataReceived = 0 ;
	state.transmitOk = 0 ;
	state.trys = 0 ;
	state.losts = 0 ;
}

void ShockBurst::init()
{
    pinMode(cePin, OUTPUT) ;
    pinMode(csnPin, OUTPUT) ;

	digitalWrite(cePin, LOW) ;
	digitalWrite(csnPin, HIGH) ;

    // Initialize spi module
    spi->begin() ;
}

void ShockBurst::config()
{
#ifdef DEBUG
	Serial.print("[nRF24L01+] channel=") ;
	Serial.println((int)channel) ;
#endif
	// Set RF channel
	setRegister(RF_CH, channel) ;

	if(payload==0)
	{
#ifdef DEBUG
		Serial.println("[nRF24L01+] dynamic payload") ;
#endif
		//Dynamic payload
		setRegisterBits(FEATURE, EN_DPL) ;
		setRegister(DYNPD, DPL_P5 | DPL_P4 | DPL_P3 | DPL_P2 | DPL_P1 | DPL_P0) ;
		//setRegister(EN_AA, ENAA_P5 | ENAA_P4 | ENAA_P3 | ENAA_P2 | ENAA_P1 | ENAA_P0) ;
	}
	else
	{
#ifdef DEBUG
		Serial.print("[nRF24L01+] static payload=") ;
		Serial.print((int)payload) ;
		Serial.println('o') ;
#endif
		for(uint8_t pipe=0 ; pipe<6 ; pipe++)
		{
			// Set length of incoming payload
			setRegister(RX_PW_P0+pipe, payload) ;
		}
	}

#ifdef DEBUG
	Serial.print("[nRF24L01+] mac size=") ;
	Serial.println((int)mac_size+2) ;
	Serial.print("[nRF24L01+] mac=") ;
	dispMac(mac) ;
	Serial.println("") ;
#endif

	//Set mac
	setRegister(SETUP_AW, mac_size) ;
	writeRegister(TX_ADDR, (uint8_t*)&mac, mac_size+2) ;

	//Enable dynamic payload and ACK
	setRegister(FEATURE, EN_DPL | EN_ACK_PAY | EN_DYN_ACK) ;

	//Set wait of 500µS and 15 retransmit
	setRegister(SETUP_RETR, B00011111) ;

	//Disable all RX
	setRegister(EN_RXADDR, 0) ;

    // Start receiver
    powerUpRx() ;
    flushRx() ;

}

void ShockBurst::setPipe(uint8_t pipe, uint8_t pipemac[5])
{
#ifdef DEBUG
	Serial.print("[nRF24L01+] [pipe ") ;
	Serial.print((int)pipe) ;
	Serial.print("] mac=") ;
	dispMac(pipemac) ;
	Serial.println("") ;
#endif
	//Enable pipe
	setRegisterBits(EN_RXADDR, (1 << pipe)) ;
	//Enable autoack
	setRegisterBits(EN_AA, (1 << pipe)) ;
	if(pipe==0)
	{
		rxmac.pipe0[0] = pipemac[0] ;
		rxmac.pipe0[1] = pipemac[1] ;
		rxmac.pipe0[2] = pipemac[2] ;
		rxmac.pipe0[3] = pipemac[3] ;
		rxmac.pipe0[4] = pipemac[4] ;
		//Set mac
		writeRegister(RX_ADDR_P0, pipemac, mac_size+2) ;
	}
	else if(pipe==1)
	{
		rxmac.pipe1[0] = pipemac[0] ;
		rxmac.pipe1[1] = pipemac[1] ;
		rxmac.pipe1[2] = pipemac[2] ;
		rxmac.pipe1[3] = pipemac[3] ;
		rxmac.pipe1[4] = pipemac[4] ;
		//Set mac
		writeRegister(RX_ADDR_P1, pipemac, mac_size+2) ;
	}
	else
	{
		rxmac.pipe[pipe-2] = pipemac[0] ;
		//Set mac
		writeRegister(RX_ADDR_P0+pipe, pipemac, 1) ;
	}

}

uint8_t ShockBurst::getRegister(uint8_t reg)
{
	uint8_t ret ;
	digitalWrite(csnPin, LOW) ;
    spi->transfer(R_REGISTER | (REGISTER_MASK & reg));
    ret = spi->transfer(reg) ;
	digitalWrite(csnPin, HIGH) ;

	return ret ;
}

void ShockBurst::setRegister(uint8_t reg, uint8_t value)
{
	digitalWrite(csnPin, LOW) ;
    spi->transfer(W_REGISTER | (REGISTER_MASK & reg));
    spi->transfer(value);
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::setRegisterBits(uint8_t reg, uint8_t mask)
{
	uint8_t val ;

	val = getRegister(reg) ;
	val |= mask ;

	digitalWrite(csnPin, LOW) ;
    spi->transfer(W_REGISTER | (REGISTER_MASK & reg));
    spi->transfer(val) ;
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::unsetRegisterBits(uint8_t reg, uint8_t mask)
{
	uint8_t val ;

	val = getRegister(reg) ;
	val &= ~mask ;

	digitalWrite(csnPin, LOW) ;
    spi->transfer(W_REGISTER | (REGISTER_MASK & reg));
    spi->transfer(val) ;
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::writeRegister(uint8_t reg, uint8_t* value, int size)
{
	digitalWrite(csnPin, LOW) ;
    spi->transfer(W_REGISTER | (REGISTER_MASK & reg));
	spi->transfer(value, size) ;
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::readRegister(uint8_t reg, uint8_t* value, int size)
{
	digitalWrite(csnPin, LOW) ;
    spi->transfer(R_REGISTER | (REGISTER_MASK & reg)) ;
	spi->read(value, size) ;
	digitalWrite(csnPin, HIGH) ;
}


void ShockBurst::send(uint8_t* data, uint8_t size)
{
	state.transmitOk = 0 ;

	digitalWrite(cePin, LOW) ;

    powerUpTx() ;       // Set to transmitter mode , Power up

    // For auto-ack to work, the pipe0 address must be set to the Tx address while the radio is transmitting.
    // The register will be set back to the original pipe 0 address when the TX_DS or MAX_RT interrupt is asserted.
	writeRegister(RX_ADDR_P0, mac, mac_size+2) ;


    digitalWrite(csnPin, LOW) ;    	// Pull down chip select
    spi->transfer(W_TX_PAYLOAD) ; 	// Write cmd to write payload
    spi->transfer(data, size) ;		// Write payload
    digitalWrite(csnPin, HIGH) ;   	// Pull up chip select

    setRegisterBits(STATUS, MAX_RT) ;

	digitalWrite(cePin, HIGH) ;  	// Start transmission
}

void ShockBurst::sendNoAck(uint8_t* data, uint8_t size)
{
	state.transmitOk = 0 ;

	digitalWrite(cePin, LOW) ;

    powerUpTx() ;       // Set to transmitter mode , Power up

    digitalWrite(csnPin, LOW) ;    			// Pull down chip select
    spi->transfer(W_TX_PAYLOAD_NO_ACK) ; 	// Write cmd to write payload
    spi->transfer(data, size) ;				// Write payload
    digitalWrite(csnPin, HIGH) ;   			// Pull up chip select

    setRegisterBits(STATUS, MAX_RT) ;

	digitalWrite(cePin, HIGH) ;  	// Start transmission
}

void ShockBurst::ack(uint8_t pipe, uint8_t* data, uint8_t size)
{
	state.transmitOk = 0 ;

	digitalWrite(cePin, LOW) ;

    powerUpTx() ;       // Set to transmitter mode , Power up

    digitalWrite(csnPin, LOW) ;    			// Pull down chip select
    spi->transfer(W_ACK_PAYLOAD | pipe) ; 			// Write cmd to write payload
    if(data!=NULL)
    	spi->transfer(data, size) ;			// Write payload
    digitalWrite(csnPin, HIGH) ;   			// Pull up chip select

    setRegisterBits(STATUS, MAX_RT) ;

	digitalWrite(cePin, HIGH) ;  	// Start transmission
}

uint8_t ShockBurst::getData(uint8_t* data)
{
	uint8_t size ;

	//Read data size
	size = getPlWidth() ;

	if(state.pipe==B111 || state.pipe==B110)
		return 0xFF ;	//No data

	if(size>32)
	{
		flushRx() ;
		return 0xFF ;
	}

	digitalWrite(csnPin, LOW) ;     	// Pull down chip select
    spi->transfer(R_RX_PAYLOAD) ;   	// Send cmd to read rx payload
    spi->read(data, size) ; 			// Read payload
    digitalWrite(csnPin, HIGH) ;     	// Pull up chip select

    if(rxFifoEmpty())
    	state.dataReceived = 0 ;
    clearStatus() ;

    return size ;
}

uint8_t ShockBurst::getPlWidth()
{
	uint8_t ret=0 ;
	digitalWrite(csnPin, LOW) ;
	spi->transfer(R_RX_PL_WID) ;
    ret = spi->transfer(0) ;
	digitalWrite(csnPin, HIGH) ;

	return ret ;
}

void ShockBurst::powerUpRx()
{
	digitalWrite(cePin, LOW) ;
	setRegisterBits(CONFIG, PWR_UP | PRIM_RX) ;
	digitalWrite(cePin, HIGH) ;
	setRegister(STATUS, TX_DS | MAX_RT) ;
}

void ShockBurst::powerUpTx()
{
	setRegisterBits(CONFIG, PWR_UP) ;
	unsetRegisterBits(CONFIG, PRIM_RX) ;
}

bool ShockBurst::txFifoFull()
{
	return getRegister(STATUS)&TX_FULL ;
}

bool ShockBurst::txFifoEmpty()
{
	return getRegister(FIFO_STATUS)&TX_EMPTY ;
}

bool ShockBurst::rxFifoEmpty()
{
	return getRegister(FIFO_STATUS)&RX_EMPTY ;
}

bool ShockBurst::rxFifoFull()
{
	return getRegister(FIFO_STATUS)&RX_FULL ;
}

bool ShockBurst::rpd()
{
	return getRegister(RPD) ;
}

void ShockBurst::clearStatus()
{
	setRegisterBits(STATUS, RX_DR | MAX_RT | TX_DS) ;
}

void ShockBurst::flushRx()
{
	digitalWrite(csnPin, LOW) ;
    spi->transfer(FLUSH_RX) ;
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::flushTx()
{
	digitalWrite(csnPin, LOW) ;
    spi->transfer(FLUSH_TX) ;
	digitalWrite(csnPin, HIGH) ;
}

void ShockBurst::clearState()
{
	state.dataReceived = 0 ;
	state.transmitOk = 0 ;
	state.transmitErr = 0 ;
	state.losts = 0 ;
	state.trys = 0 ;
	//Reset PLOS_CNT by setting RF_CH
	setRegister(RF_CH, channel) ;
}

void ShockBurst::flush()
{
	clearStatus() ;
	writeRegister(RX_ADDR_P0, rxmac.pipe0, mac_size+2) ;
	flushRx() ;
	flushTx() ;
}

void ShockBurst::nrf24l_irq()
{
    uint8_t status ;
    uint8_t observe ;

    status = getRegister(STATUS) ;
    observe = getRegister(OBSERVE_TX) ;

    state.losts = (observe & PLOS_CNT) >> 4 ;
    state.trys = (observe & ARC_CNT) ;

    if(status & RX_DR)
    {
    	state.pipe = (status & RX_P_NO) >> 1 ;
    	state.dataReceived = 1 ;
    }

    // We can get the TX_DS or the MAX_RT interrupt, but not both.
    if(status & TX_DS)
    {
        // if there's nothing left to transmit, switch back to receive mode.
    	writeRegister(RX_ADDR_P0, rxmac.pipe0, mac_size+2) ;
        powerUpRx() ;
        state.transmitOk = 1 ;
        state.transmitErr = 0 ;
    }
    else if(status & MAX_RT)
    {
        flushTx() ;

        //transmit_lock = 0;
    	writeRegister(RX_ADDR_P0, rxmac.pipe0, mac_size+2) ;
        powerUpRx() ;
        state.transmitOk = 0 ;
        state.transmitErr = 1 ;
    }

    // clear the interrupt flags.
    clearStatus() ;
}


#ifdef DEBUG
void ShockBurst::dispMac(uint8_t mac[])
{
	for(int size=mac_size+1 ; size>=0 ; size--)
	{
		int val[2]={(mac[size]&0xF0)>>4, mac[size]&0x0F} ;
		Serial.print(val[0], HEX) ;
		Serial.print(val[1], HEX) ;
	}
}

void ShockBurst::status()
{
	uint8_t addr[5] ;

	Serial.print("[nRF24L01+] CONFIG=") ;
	Serial.println(getRegister(CONFIG), BIN) ;

	Serial.print("[nRF24L01+] EN_AA=") ;
	Serial.println(getRegister(EN_AA), BIN) ;

	Serial.print("[nRF24L01+] EN_RXADDR=") ;
	Serial.println(getRegister(EN_RXADDR), BIN) ;

	Serial.print("[nRF24L01+] SETUP_AW=") ;
	Serial.println(getRegister(SETUP_AW), BIN) ;

	Serial.print("[nRF24L01+] SETUP_RETR=") ;
	Serial.println(getRegister(SETUP_RETR), BIN) ;

	Serial.print("[nRF24L01+] RF_CH=") ;
	Serial.println(getRegister(RF_CH), BIN) ;

	Serial.print("[nRF24L01+] RF_SETUP=") ;
	Serial.println(getRegister(RF_SETUP), BIN) ;

	Serial.print("[nRF24L01+] STATUS=") ;
	Serial.println(getRegister(STATUS), BIN) ;

	Serial.print("[nRF24L01+] OBSERVE_TX=") ;
	Serial.println(getRegister(OBSERVE_TX), BIN) ;

	Serial.print("[nRF24L01+] RPD=") ;
	Serial.println(getRegister(RPD), BIN) ;

	Serial.print("[nRF24L01+] FIFO_STATUS=") ;
	Serial.println(getRegister(FIFO_STATUS), BIN) ;

	Serial.print("[nRF24L01+] DYNPD=") ;
	Serial.println(getRegister(DYNPD), BIN) ;

	Serial.print("[nRF24L01+] FEATURE=") ;
	Serial.println(getRegister(FEATURE), BIN) ;



	readRegister(TX_ADDR, addr, mac_size+2) ;
	Serial.print("[nRF24L01+] TX_ADDR=") ;
	dispMac(addr) ;
	Serial.println("") ;

	readRegister(RX_ADDR_P0, addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P0=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P0=") ;
	Serial.println(getRegister(RX_PW_P0), BIN) ;

	readRegister(RX_ADDR_P1, addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P1=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P1=") ;
	Serial.println(getRegister(RX_PW_P1), BIN) ;

	readRegister(RX_ADDR_P2, addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P2=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P2=") ;
	Serial.println(getRegister(RX_PW_P2), BIN) ;

	readRegister(RX_ADDR_P3, (uint8_t*)&addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P3=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P3=") ;
	Serial.println(getRegister(RX_PW_P3), BIN) ;

	readRegister(RX_ADDR_P4, (uint8_t*)&addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P4=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P4=") ;
	Serial.println(getRegister(RX_PW_P4), BIN) ;

	readRegister(RX_ADDR_P5, (uint8_t*)&addr, mac_size+2) ;
	Serial.print("[nRF24L01+] RX_ADDR_P5=") ;
	dispMac(addr) ;
	Serial.println("") ;

	Serial.print("[nRF24L01+] RX_PW_P5=") ;
	Serial.println(getRegister(RX_PW_P5), BIN) ;
}
#endif
