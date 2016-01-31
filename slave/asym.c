#include "asym.h"
#include <system.h>

//#define IS_SLAVE

PRIVILEGED_DATA static Queue * xReqQueue = (Queue*) MEMORY_BUFF_BASE;

#ifdef IS_SLAVE
/* Array of functions pointers to hold the tasks */
PRIVILEGED_DATA static  void (* pxTasks[ NUMBER_OF_TASKS ] )( void *p );
#endif

static alt_mutex_dev * mutex;

/**
 * Initiate the Mutex
 */

bool_t xAsymMutexInit(){

	if(!altera_avalon_mutex_open(MUTEX_0_NAME)) {
		/* Failed to instantiate mutex */
		return xFalse;
	  }

	 mutex = altera_avalon_mutex_open(MUTEX_0_NAME);
	 return xTrue;
}

bool_t xAsymReqQueuInit(){
	altera_avalon_mutex_lock(mutex, 1);
	if( altera_avalon_mutex_is_mine(mutex)) {
		xReqQueue->uxNumberOfItems = 0;
		xReqQueue->xToAdd = 0;
		xReqQueue->xToServe = 0;
		altera_avalon_mutex_unlock(mutex);
		return xTrue;
	}
	else
		return xFalse;
}

bool_t xAsymSendReq( int8_t xReqValue ){

	altera_avalon_mutex_lock(mutex, 1);
	if( altera_avalon_mutex_is_mine(mutex)) {
		xReqQueue->pxItems[ xReqQueue->xToAdd ].xItemValue = xReqValue;
		xReqQueue->pxItems[ xReqQueue->xToAdd ].xServed = 0;
		xReqQueue->xToAdd++;
		xReqQueue->uxNumberOfItems++;
		altera_avalon_mutex_unlock(mutex);
		return xTrue;
	}
	else
		return xFalse;
}

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
	xReqQueue->uxNumberOfItems--;
	xReqQueue->xToServe++;
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


#endif
