#ifndef _NRF24L01_H_
#define _NRF24L01_H_

/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

/* Memory Map */
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define RPD         0x09	//CD is changed to RPD on the nRF24L01+
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define RX_ADDR_P2  0x0C
#define RX_ADDR_P3  0x0D
#define RX_ADDR_P4  0x0E
#define RX_ADDR_P5  0x0F
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define RX_PW_P1    0x12
#define RX_PW_P2    0x13
#define RX_PW_P3    0x14
#define RX_PW_P4    0x15
#define RX_PW_P5    0x16
#define FIFO_STATUS 0x17
#define DYNPD 		0x1C
#define FEATURE		0x1D




/* Instruction Mnemonics */
#define R_REGISTER    			B00000000
#define W_REGISTER    			B00100000
#define REGISTER_MASK 			B00011111
#define R_RX_PAYLOAD  			B01100001
#define W_TX_PAYLOAD  			B10100000
#define FLUSH_TX      			B11100001
#define FLUSH_RX      			B11100010
#define REUSE_TX_PL   			B11100011
#define R_RX_PL_WID   			B01100000
#define W_ACK_PAYLOAD 			B10101000
#define W_TX_PAYLOAD_NO_ACK 	B10110000
#define NOP           			B11111111




/*Data rates*/
#define RF_DR_2MPS 		B00001000
#define RF_DR_1MPS 		B00000000
#define RF_DR_250KBPS 	B00100000

#define RF_PWR_MAX 	B000000110
#define RF_PWR_HIGH B000000100
#define RF_PWR_LOW  B000000010
#define RF_PWR_MIN  B000000000

/*Auto retransmit*/
#define ARC_DISABLED 0

/*RX/TX Address field width*/
#define AW_3B 		B00000001
#define AW_4B 		B00000010
#define AW_5B 		B00000011

/*Payload data pipe size*/
#define RX_PW_NOT_USED 0




/*CONFIG register bit masks*/
#define MASK_RX_DR 	B01000000
#define MASK_TX_DS 	B00100000
#define MASK_MAX_RT B00010000
#define EN_CRC		B00001000
#define CRCO		B00000100
#define PWR_UP		B00000010
#define PRIM_RX		B00000001

/*EN_AA register bit masks*/
#define ENAA_P5		B00100000
#define ENAA_P4		B00010000
#define ENAA_P3		B00001000
#define ENAA_P2		B00000100
#define ENAA_P1		B00000010
#define ENAA_P0		B00000001

/*EN_RXADDR register bit masks*/
#define ERX_P5		B00100000
#define ERX_P4		B00010000
#define ERX_P3		B00001000
#define ERX_P2		B00000100
#define ERX_P1		B00000010
#define ERX_P0		B00000001

/*SETUP_AW register bit masks*/
#define AW			B00000011

/*SETUP_RETR register bit masks*/
#define ARD			B11110000	//Each bit is a multiple of 250µs
#define ARC			B00001111	//Max number of try on AA fail. Number between 1 and 15, 0 to disable (ARC_DISABLED)

/*RF_SETUP register bit masks*/
#define CONT_WAVE	B10000000
#define RF_DR_LOW	B00100000
#define PLL_LOCK	B00010000
#define RF_DR_HIGH	B00001000
#define RF_PWR		B00000110

/*STATUS register bist masks*/
#define RX_DR		B01000000
#define TX_DS		B00100000
#define MAX_RT		B00010000
#define RX_P_NO		B00001110
#define TX_FULL		B00000001

/*OBSERVE_TX register bit masks*/
#define PLOS_CNT	B11110000
#define ARC_CNT		B00001111

/*FIFO_STATUS register bit masks*/
#define TX_REUSE	B01000000
#define TX_FULL2	B00100000
#define TX_EMPTY	B00010000
#define RX_FULL		B00000010
#define RX_EMPTY	B00000001

/*DYNPD register bit masks*/
#define DPL_P5		B00100000
#define DPL_P4		B00010000
#define DPL_P3		B00001000
#define DPL_P2		B00000100
#define DPL_P1		B00000010
#define DPL_P0		B00000001

/*FEATURE register bit masks*/
#define EN_DPL 		B00000100
#define EN_ACK_PAY 	B00000010
#define EN_DYN_ACK 	B00000001

#define PACKET_SIZE 32

#endif
