//*************************************************//
/*Project: microSD card reader: ISR & RTOS		*/
/*Author: Ajay Rao                   		   	*/
/*Affiliation: NCSU                   		   	*/
/*Version: 1.0									*/
/*E-mail:abrao2@ncsu.edu                      	*/
//*************************************************//
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "integer.h"
#include <MKL25Z4.h>
#include "spi_io.h"
#include "sd_io.h"
#include "LEDs.h"
#include "debug.h"
 
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#endif

SD_DEV dev[1];          // Create device descriptor
uint8_t buffer[512];    // Example of your buffer data
volatile uint32_t sum = 0;
uint32_t idle_counter = 0;
uint32_t diff;
uint32_t init_val;


 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void SD_Manager (void *argument) {
 
  // ...
	// On first run, init card and write test data to given block (sector_num) in flash. 
	// Then repeatedly read the sector and confirm its contents
	//osDelay(20000);

	int i;

	DWORD sector_num = 0x23; // Manual wear leveling
  SDRESULTS res;
	init_val=0;
	diff=0;
	// Erase buffer
	for (i=0; i<SD_BLK_SIZE; i++)
		buffer[i] = 0;
	
	// Load sample data into buffer
	buffer[0] = 0xDE;
	buffer[1] = 0xAD;
	buffer[2] = 0xC0;
	buffer[3] = 0xDE;
	
	buffer[508] = 0xFE;
	buffer[509] = 0xED;
	buffer[510] = 0xCA;
	buffer[511] = 0xFE;
	init_val = idle_counter;
  if(SD_Init(dev)!=SD_OK) {
			Control_RGB_LEDs(1,0,0); // Red - init failed
			while (1)
				;
	}
	diff=idle_counter-init_val;
		init_val = idle_counter;
	// Change the data in this sector
	res = SD_Write(dev, (void*)buffer, sector_num);
	diff=idle_counter-init_val;
	if(res!=SD_OK) {
		Control_RGB_LEDs(1,0,1); // Magenta - write failed
		while (1)
			;
	}
	Control_RGB_LEDs(0,0,1);	// Blue - written ok
	init_val=idle_counter;
	while (1) { // Repeat this loop forever, ten times per second
		// Need to insert an osDelay call here 
		// erase buffer
		for (i=0; i<SD_BLK_SIZE; i++)
			buffer[i] = 0;
		// read block again
		res = SD_Read(dev, (void*)buffer, sector_num, 0, 512);
		diff=idle_counter-init_val;
		if(res==SD_OK) {
			for (i = 0, sum = 0; i < SD_BLK_SIZE; i++)
				sum += buffer[i];
		
			if (sum == 0x06DC)
				Control_RGB_LEDs(0,1,0); // Green - read was OK
			else
				Control_RGB_LEDs(1,0,0); // Red - checksum failed
		} else {
			Control_RGB_LEDs(1,0,0); // Red - read failed
		}
	}
  for (;;) {}
}
 
int main (void) {
  // System Initialization
  SystemCoreClockUpdate();
#ifdef RTE_Compiler_EventRecorder
  // Initialize and start Event Recorder
  EventRecorderInitialize(EventRecordError, 1U);
#endif
  // ...
	Init_Debug_Signals();
	Init_RGB_LEDs();
	__enable_irq();
	Init_Message_Queues();
	Control_RGB_LEDs(1,1,0);	// Yellow - starting up
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(SD_Manager, NULL, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
	for (;;) {}
}
