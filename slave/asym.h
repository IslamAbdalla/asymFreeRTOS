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
	int8_t xItemValue;			/*< The value being listed.  In most cases this is used to sort the list in descending order. */
	bool_t xServed;				/*< Might be needed when priority is used >*/
	//struct xQUEUE_ITEM * pxNext;		/*< Pointer to the next ListItem_t in the list. */
	//struct xQUEUE_ITEM * pxPrevious;	/*< Pointer to the previous ListItem_t in the list. */
	//void * pvQueue;				/*< Pointer to the list in which this list item is placed (if any). */
} QueueItem;
typedef struct xQUEUE
{
	uint8_t uxNumberOfItems;
	QueueItem pxItems[QUEUE_LENGTH];
	int8_t xToAdd;			/*< Used to walk through the list.  Points to the last item returned by a call to listGET_OWNER_OF_NEXT_ENTRY (). */
	int8_t xToServe;			/*< Used to walk through the list.  Points to the last item returned by a call to listGET_OWNER_OF_NEXT_ENTRY (). */
} Queue;

bool_t xAsymMutexInit();
bool_t xAsymReqQueuInit();
bool_t xAsymSendReq( int8_t xReqValue );
int8_t xAsymGetReq( int8_t xIndex );
bool_t xAsymTaskCreate();


#endif /* ASYM_H */
