#ifndef ASYM_H
#define ASYM_H

#include <altera_avalon_mutex.h>
#include <stdint.h>

#define PRIVILEGED_DATA
#define QUEUE_LENGTH 100
typedef uint32_t baseType;

// Boolean
typedef char bool_t;
#define xTrue 1
#define xFalse 0

typedef struct xQUEUE_ITEM
{
	baseType xItemValue;			/*< The value being listed.  In most cases this is used to sort the list in descending order. */
	struct xQUEUE_ITEM * pxNext;		/*< Pointer to the next ListItem_t in the list. */
	struct xQUEUE_ITEM * pxPrevious;	/*< Pointer to the previous ListItem_t in the list. */
	void * pvQueue;				/*< Pointer to the list in which this list item is placed (if any). */
	bool_t xServed;				/*< Might be needed when priority is used >*/
} QueueItem;
typedef struct xQUEUE
{
	baseType uxNumberOfItems;
	QueueItem pxItems[QUEUE_LENGTH];
	baseType pxIndex;			/*< Used to walk through the list.  Points to the last item returned by a call to listGET_OWNER_OF_NEXT_ENTRY (). */
} Queue;



#endif /* ASYM_H */
