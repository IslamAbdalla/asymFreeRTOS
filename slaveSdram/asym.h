#ifndef ASYM_H
#define ASYM_H

#include <altera_avalon_mutex.h>
#include <stdint.h>

#define PRIVILEGED_DATA
#define QUEUE_LENGTH 16
#define NUMBER_OF_TASKS 16

// Boolean
typedef char bool_t;
#define xTrue 1
#define xFalse 0

typedef enum {
	Task0,
	Task1,
	Task2,
	Task3,
	Task4,
	Task5,
	Task6
} xTaskIndex_t;

typedef struct xQUEUE_ITEM
{
	int8_t xItemValue;			/*< The value being sent. */
	bool_t xServed;				/*< Might be needed when priority is used >*/
	void * pvSemaphore;				/*< Pointer to the semaphore the control the execution of the task that is waiting for it to finish. */
} QueueItem;
typedef struct xQUEUE
{
	uint8_t uxNumberOfItems;
	QueueItem pxItems[QUEUE_LENGTH];
	int8_t xToAdd;			/*< Holds the value of the next empty location */
	int8_t xToServe;			/*< Holds the value of the task that is to be executed. */
} Queue;

/**
 * Initiate the Mutex
 */
bool_t xAsymMutexInit();

/**
 * Initiate the request queue by setting its variables to zero.
 * This functions also adds vAsymSemaphorePolling as a FreeRTOS task
 *
 * @return xTrue if the initiation is successful. xFalse otherwise
 */
bool_t xAsymReqQueuInit();

/**
 * Send a request from master to slave with a task number. The function uses
 * busy waiting if the queue is full and does not return until the request
 * is placed on the queue
 * @param xReqValue The task number. TODO: should be changed to xTaskIndex_t
 * enum
 *
 * @return xTrue if the request is successful. xFalse otherwise
 */
bool_t xAsymSendReq( int8_t xReqValue , bool_t xReturn );

/**
 * Returns the value of a request from the request queue.
 * @param xIndex the request index
 */
int8_t xAsymGetReq( int8_t xIndex );

/*
 *
 */
void vAsymSemaphorePolling(void *p );

/**
 *
 */
bool_t xAsymReqQueuInit();

/**
 *
 */
bool_t xAsymReqQueueNotEmpty();


// Master-specific functions:

/**
 *
 */
void vAsymUpdateFinishedReq();
// End of master-specific


// Slave-specific
/**
 *
 */
bool_t xAsymTaskCreate();
/**
 *
 */
void vAsymServeReq(int8_t xToServe);
/**
 *
 */
void vAsymStartScheduler();

/**
 * Loops through the requests, and check if a request can run
 *  then gives the semaphore of the requested task so the request can
 *  be serviced.
 */
void vAsymUpdateSentReq();
/**
 *
 */
void vAsymServeRequest(int8_t ucRequestedTask);
// End of slave-specific

#endif /* ASYM_H */
