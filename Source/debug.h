#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#define MASK(x) (1UL << (x))

// Debug Signals on port B
#define DBG_1 1 	
#define DBG_2 2	  
#define DBG_3 3		
#define DBG_4 8		
#define DBG_5 9		
#define DBG_6 10  
#define DBG_7 11

#define DBG_SPI_RW 				DBG_1  
#define DBG_SD_READ 			DBG_2 
#define DBG_SD_WRITE			DBG_3 
#define DBG_SD_SEND_CMD 	DBG_4 
#define DBG_SD_INIT 			DBG_5 
#define DBG_ISR						DBG_6
#define DBG_IDLE	 				DBG_7   

void Init_Debug_Signals(void);

extern uint32_t idle_counter;
extern osMessageQueueId_t spi_msgq_id;
#endif // DEBUG_H
