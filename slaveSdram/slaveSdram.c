/* 
 * "Small Hello World" example. 
 * 
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example 
 * designs. It requires a STDOUT  device in your system's hardware. 
 *
 * The purpose of this example is to demonstrate the smallest possible Hello 
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard 
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete 
 * description.
 * 
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION 
 *      This removes software exception handling, which means that you cannot 
 *      run code compiled for Nios II cpu with a hardware multiplier on a core 
 *      without a the multiply unit. Check the Nios II Software Developers 
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks  
 *      support for buffering, file IO, floating point and getch(), etc. 
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program 
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */


#include "altera_avalon_pio_regs.h"
#include "altera_avalon_mutex.h"
#include "system.h"
#include "unistd.h"

// FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Asym
#define IS_SLAVE
#include "asym.h"

#define speedy 10

void xZerothTask( void * data){
	alt_printf("Task 0 is running: ");
	usleep(1500000/speedy);
	alt_printf("Done\n");
}

void xFirstTask( void * data){
	alt_printf("Task 1 is running: ");
	usleep(1500000/speedy);
	alt_printf("Done\n");
}
void xSecondTask( void * data){
	alt_printf("Task 2 is running: ");
	usleep(2500000/speedy);
	alt_printf("Done\n");
}

void xThirdTask( void * data){
	alt_printf("Task 3 is running: ");
	usleep(3500000/speedy);
	alt_printf("Done\n");
}

void xFourthTask( void * data){
	alt_printf("Task 4 is running: ");
	usleep(2500000/speedy);
	alt_printf("Done\n");
}

void xFifthTask( void * data){
	alt_printf("Task 5 is running: ");
	usleep(1500000/speedy);
	alt_printf("Done\n");
}

void xSixthTask( void * data){
	alt_printf("Task 6 is running: ");
	usleep(3500000/speedy);
	alt_printf("Done\n");
}

int main()
{ 
	  //alt_mutex_dev* mutex;
	 // int *message;
	 // message =(int*) MEMORY_BUFF_BASE;
//
//	  if(!altera_avalon_mutex_open(MUTEX_0_NAME)) {
//		  alt_printf("Error: could not open the mutex\n");
//		  return 0;
//	  }
//	  mutex = altera_avalon_mutex_open(MUTEX_0_NAME);

	  xAsymMutexInit();
	  xAsymReqQueuInit();

	  alt_putstr("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	  alt_putstr("CPU 1 started\n");


  int status = 0;
  /* Event loop never exits. */
//  while (status < 3 ){
//	  alt_printf("Waiting: %x!\n",status++ );
//	  usleep(900000);
//  }

  xAsymTaskCreate(xZerothTask , Task0 );
  xAsymTaskCreate(xFirstTask , Task1 );
  xAsymTaskCreate(xSecondTask , Task2 );
  xAsymTaskCreate(xThirdTask , Task3 );
  xAsymTaskCreate(xFourthTask , Task4 );
  xAsymTaskCreate(xFifthTask , Task5 );
  xAsymTaskCreate(xSixthTask , Task6 );


//  alt_putstr("Added tasks!\n");

  status = 0;
//  while (status < 3 ){
//	  alt_printf("Got: %x in index %x!\n",xAsymGetReq(status), status++ );
////	  usleep(200000);
//  }
  vAsymStartScheduler();
  while (1){};

//	  altera_avalon_mutex_trylock(mutex, 1);
//		  	  if( altera_avalon_mutex_is_mine(mutex)) {
//
//		  		alt_printf("Got value: %x!\n",*message );
//		  		 usleep(2900000);
//		  		  altera_avalon_mutex_unlock(mutex);
//		  	  }
//		  	  else {
//			  		alt_printf("Sorry, got NOTHING!\n");
//
//		  	  }
//
//
//	  IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE,  status++);
//	  //alt_printf("Hello %x\n",status);
//  	  usleep(900000);
//  }
  return 0;
}
