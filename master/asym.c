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
	xReqQueue->uxNumberOfItems = 0;
	xReqQueue->xToAdd = 0;
	xReqQueue->xToServe = 0;
	return xTrue;
}

bool_t xAsymSendReq( int8_t xReqValue ){
	/* TODO: add mutex */
	xReqQueue->pxItems[ xReqQueue->xToAdd ].xItemValue = xReqValue;
	xReqQueue->pxItems[ xReqQueue->xToAdd ].xServed = 0;
	xReqQueue->xToAdd++;
	xReqQueue->uxNumberOfItems++;
	return xTrue;
}

int8_t xAsymGetReq( int8_t xIndex ) {
	return (int8_t) ( xReqQueue->pxItems[ xIndex ].xItemValue );
}



