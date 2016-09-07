/*
 * file : os_wrapper_FreeRTOS.c
 *
 *
 *
 *
 *
 *
 */


#include "_project.h"

#include "os_wrapper.h"

extern uint8_t stack_storage_area[];
static uint16_t curr_stack_storage_pointer = ATOMTHREADS_CONFIG_IDLE_STACK_SIZE_BYTES;

/* function : os_create_queue_atomthreads
 *
 */
void os_init_atomthreads()
{
    int8_t status;
    status = atomOSInit(&stack_storage_area[ATOMTHREADS_CONFIG_IDLE_STACK_SIZE_BYTES - 1],
    		ATOMTHREADS_CONFIG_IDLE_STACK_SIZE_BYTES);


    /**
     * Note: to protect OS structures and data during initialisation,
     * interrupts must remain disabled until the first thread
     * has been restored. They are reenabled at the very end of
     * the first thread restore, at which point it is safe for a
     * reschedule to take place.
     */

    /* trap if  problem with os init */
    while (status != ATOM_OK);

}

#if ATOMTHREADS_CONFIG_QUEUES_STORAGE_SIZE>0

uint8_t queue_storage[ATOMTHREADS_CONFIG_QUEUES_STORAGE_SIZE];
ATOM_QUEUE queues[ATOMTHREADS_CONFIG_MAX_NUM_OF_QUEUES];

/* function : os_create_queue_atomthreads
 *
 */
os_queue_t os_create_queue_atomthreads(uint8_t num_of_elements ,uint8_t size_of_element)
{

	static uint8_t curr_queue = 0;
	static uint16_t curr_storage_pointer=0;
	uint8_t queueSize ;
	ATOM_QUEUE	*curAtomQueue;

	queueSize = num_of_elements*size_of_element;
	curAtomQueue = &queues[curr_queue++];

	// trap if num of queues is more then defined
	while(ATOMTHREADS_CONFIG_MAX_NUM_OF_QUEUES < curr_queue);

	atomQueueCreate (curAtomQueue ,
			&queue_storage[curr_storage_pointer], size_of_element, num_of_elements);

	curr_storage_pointer += queueSize;

	// trap if storage of queues exceed then defined
	while(ATOMTHREADS_CONFIG_QUEUES_STORAGE_SIZE < curr_storage_pointer );


	return curAtomQueue;

}

#endif



static ATOM_TCB tcb_info[ATOMTHREADS_CONFIG_MAX_NUM_OF_TASKS];

/* function : os_create_task_atomthreads
 *
 */
void *os_create_task_atomthreads( void *taskFunction,
		void *taskFunctionParam , uint16_t stackSize , uint8_t priority)
{
	static uint8_t curr_tcb = 0;
	curr_stack_storage_pointer += stackSize;

	while(ATOMTHREADS_CONFIG_MAX_NUM_OF_TASKS == curr_tcb); // trap if num of tasks is more then defined

	// trap if storage of task stack exceed then defined
	while(ATOMTHREADS_CONFIG_STACK_STORAGE_SIZE < curr_stack_storage_pointer );


	if (ATOM_OK != atomThreadCreate(&tcb_info[curr_tcb++], priority , taskFunction, taskFunctionParam,
			&stack_storage_area[curr_stack_storage_pointer-1] , stackSize) 			)
	{
		while (1)
		{
			 // stuck here
		}
	}


	return (void*) 1;// return not NULL
}


/* function : os_delay_ms_atomthreads
 *
 */
void os_delay_ms_atomthreads(uint32_t ms)
{
	atomTimerDelay(ms);
}
