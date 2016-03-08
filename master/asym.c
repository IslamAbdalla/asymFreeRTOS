#include "asym.h"
#include <system.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//#define IS_SLAVE
	#define IS_MASTER

PRIVILEGED_DATA static Queue * xReqQueue = (Queue*) MEMORY_BUFF_BASE;

#ifdef IS_SLAVE
/* Array of functions pointers to hold the tasks */
PRIVILEGED_DATA static void (* pxTasks[ NUMBER_OF_TASKS ] )( void *p );

typedef struct xTO_SERVE_QUEUE
{
	int8_t xTaskToServe[QUEUE_LENGTH];
	bool_t xOccupied[QUEUE_LENGTH];
	void * pvSemaphore[QUEUE_LENGTH];
} ToServeQueue;

PRIVILEGED_DATA static ToServeQueue xToServeQueue;
#endif // IS_SLAVE

static alt_mutex_dev * mutex;

bool_t xAsymMutexInit(){
	if(!altera_avalon_mutex_open(MUTEX_0_NAME)) {
		/* Failed to instantiate mutex */
		return xFalse;
	}

	mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
	return xTrue;
}


void vAsymSemaphorePolling(void *p ){
	while(1){
#ifdef IS_MASTER
		vAsymUpdateFinishedReq();
#endif // IS_MASTER


#ifdef IS_SLAVE
		vAsymUpdateSentReq();
#endif // IS_MASTER

		vTaskDelay(200);
	}
}



bool_t xAsymReqQueuInit(){
	altera_avalon_mutex_lock(mutex, 1);
	if( altera_avalon_mutex_is_mine(mutex)) {
		xReqQueue->uxNumberOfItems = 0;
		xReqQueue->xToAdd = 0;
		xReqQueue->xToServe = 0;
		altera_avalon_mutex_unlock(mutex);
		int8_t ucIndex;
#ifdef IS_MASTER

		for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
			xReqQueue->pxItems[ ucIndex ].xServed = -1;
		}
		/* Creating a polling task for the semaphore */
		xTaskCreate( vAsymSemaphorePolling,"polling", 356, NULL, configMAX_PRIORITIES, NULL  );
#endif // IS_MASTER

#ifdef IS_SLAVE
		for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
			xToServeQueue.xTaskToServe[ucIndex] = NULL;
			xToServeQueue.xOccupied[ucIndex] = 0;
		}
		/* Creating a polling task for the semaphore */
		xTaskCreate( vAsymSemaphorePolling,"polling", 356, NULL, configMAX_PRIORITIES, NULL  );

#endif // IS_MASTER
		return xTrue;
	}
	else
		return xFalse;
}

#ifdef IS_MASTER

bool_t xAsymSendReq( int8_t xReqValue, bool_t xReturn){
	xSemaphoreHandle xSemaphore = xSemaphoreCreateBinary();

	altera_avalon_mutex_lock(mutex, 1);
	if( altera_avalon_mutex_is_mine(mutex)) {
		/* Busy waiting while QUEUE_LENGTH gets less than uxNumberOfItems */
		while(!(QUEUE_LENGTH > xReqQueue->uxNumberOfItems)){
			altera_avalon_mutex_unlock(mutex);
			vTaskDelay(10);
			altera_avalon_mutex_lock(mutex, 1);
		}
		//taskENTER_CRITICAL();
		xReqQueue->pxItems[ xReqQueue->xToAdd ].xItemValue = xReqValue;
		xReqQueue->pxItems[ xReqQueue->xToAdd ].xServed = 0;
		xReqQueue->pxItems[ xReqQueue->xToAdd ].pvSemaphore = (void*) &xSemaphore;
		xReqQueue->xToAdd = (QUEUE_LENGTH == (xReqQueue->xToAdd + 1))? 0: xReqQueue->xToAdd + 1;
		xReqQueue->uxNumberOfItems++;
		altera_avalon_mutex_unlock(mutex);

		/* Check ucReturn */
		if(xReturn){
			/* Function return immediately */
			vSemaphoreDelete( xSemaphore );
			xReqQueue->pxItems[ xReqQueue->xToAdd ].pvSemaphore = NULL;
			return xTrue;
		}
		/* Task should block waiting for the request to be served */
		//alt_printf("Now I am blocked. Smphr: %x\nuxNumberOfItems: %x\n\txToAdd: %x\n", &xSemaphore,xReqQueue->uxNumberOfItems, xReqQueue->xToAdd);
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		//taskEXIT_CRITICAL();
		vSemaphoreDelete( xSemaphore );
		return xTrue;

	}
	else
		return xFalse;
}

void vAsymUpdateFinishedReq(){
	int8_t ucIndex;
	altera_avalon_mutex_lock(mutex, 1);
	for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
		if(xReqQueue->pxItems[ ucIndex ].xServed == 1){
			//	alt_printf("ucIndex: %x\txServed: %x\n",ucIndex, xReqQueue->pxItems[ ucIndex ].xServed);
			if(xReqQueue->pxItems[ ucIndex ].pvSemaphore != NULL ){
			xSemaphoreHandle* xSemaphore = (xSemaphoreHandle *) ( xReqQueue->pxItems[ ucIndex ].pvSemaphore );
			xSemaphoreGive( *xSemaphore);
			}
			xReqQueue->pxItems[ ucIndex ].xServed = -1;

		}
	}
	altera_avalon_mutex_unlock(mutex);
}
#endif // IS_MASTER

int8_t xAsymGetReq( int8_t xIndex ) {
	int8_t xReturnValue;
	altera_avalon_mutex_lock(mutex, 1);
	if( altera_avalon_mutex_is_mine(mutex)) {
		xReturnValue = (int8_t) ( xReqQueue->pxItems[ xIndex ].xItemValue );
		altera_avalon_mutex_unlock(mutex);
		return xReturnValue;
	}
	else
		return xFalse;
}
bool_t xAsymReqQueueNotEmpty(){
	altera_avalon_mutex_lock(mutex, 1);
	bool_t xNotEmpty = xReqQueue->uxNumberOfItems;
	altera_avalon_mutex_unlock(mutex);
	return xNotEmpty;
}

#ifdef IS_SLAVE
bool_t xAsymTaskCreate( void (* pxTask )( void *p ) , xTaskIndex_t xTaskIndex){

	pxTasks[ xTaskIndex ] = pxTask;
	return xTrue;
}

void vAsymServeReq(int8_t xToServe){

	altera_avalon_mutex_lock(mutex, 1);
	int8_t xItemValue =  xReqQueue->pxItems[ xToServe ].xItemValue;
	altera_avalon_mutex_unlock(mutex);

	void * pvData;
	( *pxTasks[ xItemValue ] )( pvData);

	altera_avalon_mutex_lock(mutex, 1);
	xReqQueue->pxItems[ xToServe ].xServed = 1;
	xReqQueue->uxNumberOfItems--;
	xReqQueue->xToServe = (QUEUE_LENGTH == (xReqQueue->xToServe + 1 ))? 0: xReqQueue->xToServe + 1;
	altera_avalon_mutex_unlock(mutex);

}

void vAsymStartScheduler(){
	while(1){
		while(xAsymReqQueueNotEmpty() ){
			altera_avalon_mutex_lock(mutex, 1);
			int8_t xToServe = xReqQueue->xToServe;
			altera_avalon_mutex_unlock(mutex);
			vAsymServeReq( xToServe );
		}
	}
}

void vAsymUpdateSentReq(){
	// Here should give semaphores to allow tasks to run
	int8_t ucIndex, ucIndexNest;
	altera_avalon_mutex_lock(mutex, 1);
	for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
		if(xReqQueue->pxItems[ ucIndex ].xServed == 0){
			for (ucIndexNest = 0; ucIndexNest < QUEUE_LENGTH ; ucIndexNest++){
				if(xToServeQueue.xOccupied[ucIndexNest]  && xToServeQueue.xTaskToServe[ucIndexNest] == xReqQueue->pxItems[ucIndex].xItemValue){
					// Match found
					//alt_printf("ucIndex: %x, ucIndexNest: %x, TaskToServ: %x\n",ucIndex,ucIndexNest, xToServeQueue.xTaskToServe[ucIndexNest]);
					xSemaphoreHandle* xSemaphore = (xSemaphoreHandle *) ( xToServeQueue.pvSemaphore[ucIndexNest] );
					xSemaphoreGive( *xSemaphore);
					xReqQueue->pxItems[ ucIndex ].xServed = 1;
					xToServeQueue.xOccupied[ucIndexNest] = 0;
				}
			}
		}
	}
	altera_avalon_mutex_unlock(mutex);
}

void vAsymServeRequest(int8_t ucRequestedTask){
	// Loop through xToServeRequest. If request is not found, create a new semaphore.
	int8_t ucIndex;

	taskENTER_CRITICAL();
	for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
		if( xToServeQueue.xOccupied[ucIndex]  && xToServeQueue.xTaskToServe[ucIndex] == ucRequestedTask ){
			//alt_printf("Found task in queue \n");
			xSemaphoreHandle* xSemaphore = (xSemaphoreHandle *) ( xToServeQueue.pvSemaphore[ucIndex] );
			xSemaphoreTake( *xSemaphore, portMAX_DELAY);

			vSemaphoreDelete( *xSemaphore );
			return;
		}
	}
	// If not found
	// Find an empty place:
	for (ucIndex = 0; ucIndex < QUEUE_LENGTH ; ucIndex++){
		if( !xToServeQueue.xOccupied[ucIndex]) break;
	}
	if(ucIndex >= QUEUE_LENGTH) {
		// No empty location.
		taskEXIT_CRITICAL();
		return;
	}
	//alt_printf("Found Empty location at: %x  \n", ucIndex);
	xToServeQueue.xOccupied[ucIndex] = 1;
	xToServeQueue.xTaskToServe[ucIndex] = ucRequestedTask;
	xSemaphoreHandle xSemaphore = xSemaphoreCreateBinary();
	xToServeQueue.pvSemaphore[ucIndex] = (void*) &xSemaphore;
	xSemaphoreTake( xSemaphore, portMAX_DELAY);

	vSemaphoreDelete( xSemaphore );
	taskEXIT_CRITICAL();

}
#endif
