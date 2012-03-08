#ifndef _nRF24L01SB_H_
#define _nRF24L01SB_H_

#include <Arduino.h>
#include <nRF24L01.h>
#include <SpiDriver.h>

class ShockBurst
{
public:

	struct
	{
		volatile uint8_t dataReceived:1 ;
		volatile uint8_t transmitOk:1 ;
		volatile uint8_t transmitErr:1 ;
		volatile uint8_t pipe:3 ;

		//Error control data
		volatile uint8_t losts:4 ;	//Number of lost packets
		volatile uint8_t trys:4 ;	//Number of try during one packet send
	} state ;


	struct
	{
		/**
		 * Pipe 0 and pipe 1 have once up to 5 bytes mac adress.
		 */
		uint8_t pipe0[5] ;
		uint8_t pipe1[5] ;
		/**
		 * Pipe 2 to 5 shares the 4th upper bytes with pipe1 and has only the first byte configurable.
		 */
		uint8_t pipe[4] ;
	} rxmac ;

	/*
	 * CE Pin controls RX / TX, default 8.
	 */
	uint8_t cePin ;

	/*
	 * CSN Pin Chip Select Not, default 7.
	 */
	uint8_t csnPin ;

	/*
	 * Spi interface (must extend spi).
	 * Hardware SPI by default.
	 */
	SpiDriver *spi ;

	/**
	 * Payload length. Values 1 to 32 are valid for static payload.
	 * 0 indicates a dynamic payload.
	 * 0 default value.
	 */
	uint8_t payload ;

	/**
	 * Channel. 0 to 127.
	 * 100 default value.
	 */
	uint8_t channel ;

	/**
	 * Mac adress of the device.
	 * 0x1F1F1F1F1F default value.
	 */
	uint8_t mac[5] ;

	/**
	 * Width of mac address.
	 * 5 bytes default value. Possible values: see AW_xB enumeration.
	 */
	uint8_t mac_size ;


	ShockBurst() ;

	/**
	 * Init of the nrf24l01+. Must be called once at the beginning of the init part.
	 */
	void init() ;

	/**
	 * Set whole nrf24l01+ configuration. Must be called after each configuration change.
	 */
	void config() ;

	/**
	 * Add a new pipe.
	 * @param pipe number. Valid range is from 0 to 5.
	 * @param pipemac mac adress of the pipe. Note that only pipe 0 and one have a complete mac.
	 * for pipe 2 to 5 only last byte is used and the 4 first bytes are taken from pipe 1.
	 */
	void setPipe(uint8_t pipe, uint8_t pipemac[5]) ;

	/**
	 * Send data.
	 * @param data buffer to send.
	 * @param size size of the data buffer. Data buffer must be of 1 to 32 bytes maximum.
	 */
	void send(uint8_t* data, uint8_t size) ;

	/**
	 * Send data with no waited ACK.
	 * @param data buffer to send.
	 * @param size size of the data buffer. Data buffer must be of 1 to 32 bytes maximum.
	 */
	void sendNoAck(uint8_t* data, uint8_t size) ;

	/**
	 * Send and ACK on a pipe.
	 * @param pipe pipe to use for sending ACK.
	 * @param data associated buffer to send. May be NULL if no payload associated with ACK.
	 * @param size size of the data buffer. Data buffer must be of 1 to 32 bytes maximum.
	 */
	void ack(uint8_t pipe, uint8_t* data, uint8_t size) ;

	/**
	 * Read data for the nrf fifo.
	 * @param data buffer to fill with data, must be at least 32 bytes len.
	 * @param pipe get the pipe number of the data read.
	 * @return number of bytes read.
	 * If the receive buffer is empty state.dataReceived is set to 0 on exit.
	 * The pipe containing the read buffer is set in state.pipe.
	 */
	uint8_t getData(uint8_t* data) ;

	/**
	 * Read the payload width at the top of the RX payload.
	 */
	uint8_t getPlWidth() ;

	/**
	 * Switch to RX mode so the nrf is ready to receive data.
	 */
	void powerUpRx() ;

	/**
	 * Switch to TX mode so the nrf is ready to send data contained in the payload.
	 */
	void powerUpTx() ;

	/**
	 * Indicate if TX fifo is full.
	 * @return true if full.
	 */
	bool txFifoFull() ;

	/**
	 * Indicate if TX fifo is empty.
	 * @return true if empty.
	 */
	bool txFifoEmpty() ;

	/**
	 * Indicate if RX fifo is full.
	 * @return true if full.
	 */
	bool rxFifoFull() ;

	/**
	 * Indicate if RX fifo is empty.
	 * @return true if empty.
	 */
	bool rxFifoEmpty() ;

	/**
	 * Indicates if something is beeing curently sent on the channel. Usefull to check
	 * if another nrf is already sending data to avoid colliding.
	 * @return true if the channel is already occupied.
	 */
	bool rpd() ;

	/**
	 * Clear status.
	 */
	void clearStatus() ;

	/**
	 * Clear state.
	 */
	void clearState() ;

	/**
	 * Flush RX fifo.
	 */
	void flushRx() ;

	/**
	 * Flush TX fifo.
	 */
	void flushTx() ;

	/**
	 * flush RX fifo and TX fifo and clear state.
	 */
	void flush() ;

	/**
	 * this function must be called inside the IRQ function for the nrf.
	 */
	void nrf24l_irq() ;


	/**
	 * Read a 8 bits register.
	 * @param reg Register number.
	 * @return Register value. No error control is made.
	 */
	uint8_t getRegister(uint8_t reg) ;

	/**
	 * Sets a 8 bits register.
	 * @param reg Register number.
	 * @param value Value to set.
	 */
	void setRegister(uint8_t reg, uint8_t value) ;

	/**
	 * Sets bits in a 8 bits register.
	 * @param reg Register number.
	 * @param mask bit mask, only bits at 1 will be set.
	 */
	void setRegisterBits(uint8_t reg, uint8_t mask) ;

	/**
	 * Unsets bits in a 8 bits register.
	 * @param reg Register number.
	 * @param mask bit mask, only bits at 1 will be unset.
	 */
	void unsetRegisterBits(uint8_t reg, uint8_t mask) ;

	/**
	 * Write a buffer inside a register.
	 * @param reg Register number.
	 * @param data Data to write.
	 * @param size Size of the data to write.
	 */
	void writeRegister(uint8_t reg, uint8_t* data, int size) ;

	/**
	 * Read a register content inside a buffer.
	 * @param reg Register number.
	 * @param data Data to read.
	 * @param size Size of the data to read.
	 */
	void readRegister(uint8_t reg, uint8_t* data, int size) ;

#ifdef DEBUG
	void dispMac(uint8_t mac[]) ;
	void status() ;
#endif

} ;

#endif
