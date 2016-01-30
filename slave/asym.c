#include "asym.h"
#include <system.h>

PRIVILEGED_DATA static Queue * xReqQueue = (Queue*) MEMORY_BUFF_BASE;

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



