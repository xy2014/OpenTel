#ifndef _BK242X_MAP_H_
#define _BK242X_MAP_H_

/****************************************************************/ 
//SPI(Bk2423) commands 
 #define 	BK_READ_REG         0x00  // Define read command to register 
 #define	BK_WRITE_REG        0x20  // Define write command to register 
 #define	RD_RX_PLOAD	        0x61  // Define RX payload register address 
 #define	WR_TX_PLOAD	        0xA0  // Define TX payload register address 
 #define	FLUSH_TX	          0xE1  // Define flush TX register command 
 #define	FLUSH_RX	          0xE2  // Define flush RX register command 
 #define	REUSE_TX_PL	        0xE3  // Define reuse TX payload register command 
 #define	W_TX_PAYLOAD_NOACK	0xb0 
 #define	W_ACK_PAYLOAD	      0xa8 
 #define	ACTIVATE	          0x50 
 #define	R_RX_PL_WID	        0x60 
 #define	BK_NOP		          0xFF  // Define No Operation, might be used to read status register 

/***************************************************/
// SPI(Bk2423) registers(addresses) 
 #define	CONFIG							0x00  // 'Config' register address 
		#define	bMaskAllIRQ				0x70	//01110000b
		#define	bEnCrc						0x08	//00001000b
		#define	bCrc1byte					0			//00000000b
		#define	bCrc2byte					0x04	//00000100b
		#define	bPwrUp						0x02	//00000010b
		#define	bPRX						  0x01	//00000001b
		#define	bPTX						  0			//00000000b
 #define	EN_AA	              0x01  //'Enable Auto Acknowledgment' register address 
 #define	EN_RXADDR	          0x02  //'Enabled RX addresses' register address 
 #define	SETUP_AW	          0x03  //'Setup address width' register address 
 #define	SETUP_RETR	        0x04  //'Setup Auto. Retrans' register address 
 #define	RF_CH		            0x05  //'RF channel' register address 
 #define	RF_SETUP	          0x06  //'RF setup' register address 
 #define	RF_STATUS	          0x07  //'Status' register address 
		#define	bRXDR							0x40	//01000000b
		#define	bTXDS							0x20	//00100000b
		#define	bMAXRT						0x10	//00010000b
 #define	OBSERVE_TX					0x08  //'Observe TX' register address 
 #define	CD									0x09  //'Carrier Detect' register address 
 #define	RX_ADDR_P0 					0x0A  //'RX address pipe0' register address 
 #define	RX_ADDR_P1					0x0B  //'RX address pipe1' register address 
 #define	RX_ADDR_P2					0x0C  //'RX address pipe2' register address 
 #define	RX_ADDR_P3 					0x0D  //'RX address pipe3' register address 
 #define	RX_ADDR_P4 					0x0E  //'RX address pipe4' register address 
 #define	RX_ADDR_P5					0x0F  //'RX address pipe5' register address 
 #define	TX_ADDR 						0x10  //'TX address' register address 
 #define	RX_PW_P0						0x11  //'RX payload width, pipe0' register address 
 #define	RX_PW_P1						0x12  //'RX payload width, pipe1' register address 
 #define	RX_PW_P2						0x13  //'RX payload width, pipe2' register address 
 #define	RX_PW_P3						0x14  //RX payload width, pipe3' register address 
 #define	RX_PW_P4		        0x15  //'RX payload width, pipe4' register address 
 #define	RX_PW_P5		        0x16  //'RX payload width, pipe5' register address 
 #define	FIFO_STATUS		      0x17  //'FIFO Status Register' register address 
 
 #define	DYNPD			          0x1c
 #define	FEATURE			        0x1d
 
 //bank1
 #define	RAMP 			          0x0e
 
//============================================================================
//				系统自定义常量
//============================================================================
 #define	BANK0			          0			//00000000b
 #define	BANK1			          0x80		//10000000b
 				  
 #define	CONFIG_INIT		      0x0f  		//Enb 中断、2bytes CRC,RX,Power On

#endif	/*#ifndef _BK242X_MAP_H_*/


