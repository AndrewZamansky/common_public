/*
 * file : os_wrapper_linux.c
 *
 *
 *
 */


#include "_project.h"

#ifdef CONFIG_TEST_TASK_STACK
	#define DEBUG
	#include "PRINTF_api.h"
#endif

#include "os_wrapper.h"

#ifdef CONFIG_INCLUDE_HEARTBEAT
#include "heartbeat_api.h"
#endif


static struct dev_desc_t * l_timer_dev = NULL;

static struct dev_desc_t * l_heartbeat_dev = NULL;


typedef void *(*thread_func_t)(void*);

void *os_create_task(char *taskName,
		void (*taskFunction)(void *apParam),
		void *taskFunctionParam , uint16_t stackSize , uint8_t priority)
{
	int err;
	pthread_t  thread_id = 0;

	err = pthread_create(
			&thread_id, NULL, (thread_func_t)taskFunction, taskFunctionParam);
	if (err != 0)
	{
		printf("\ncan't create thread :[%s]\n", strerror(err));
	}

	return (void*)thread_id;
}

#define MAX_NUM_OF_ALLOWED_QUEUES  10
static os_queue_t  queues_in_use[MAX_NUM_OF_ALLOWED_QUEUES] = {NULL};

os_queue_t os_create_queue(uint32_t num_of_elements, uint32_t size_of_elements)
{
	os_queue_t os_queue;
	struct mq_attr attr;
	mqd_t mq;
	uint32_t i;
	char name[16] = {0};

	for (i = 0; i < MAX_NUM_OF_ALLOWED_QUEUES; i++)
	{
		if ( NULL == queues_in_use[i])
		{
			break;
		}
	}
	if (MAX_NUM_OF_ALLOWED_QUEUES == i)
	{
		printf("\n no free queue\n");
		return NULL;
	}

	snprintf(name, 16, "/queue%d", i);
	os_queue =
			(struct os_wrapper_queue *)malloc(sizeof(struct os_wrapper_queue));
	queues_in_use[i] = os_queue;

	if (NULL == os_queue)
	{
		printf("cannot allocate memory for queue\n");
		return NULL;
	}
	attr.mq_flags = 0;
	attr.mq_maxmsg = num_of_elements;
	attr.mq_msgsize = size_of_elements;
	attr.mq_curmsgs = 0;

	mq_unlink(name);
	mq = mq_open(name, O_CREAT | O_RDWR | O_EXCL, 0644, &attr);
	if (-1 == mq)
	{
		printf("cannot open queue\n");
		return NULL;
	}

	os_queue->mq = mq;
	os_queue->element_size = size_of_elements;

	return os_queue;

}


uint8_t os_queue_send_immediate(os_queue_t queue,const  void * pData  )
{
	uint8_t retVal;
	mqd_t  mq;
	struct mq_attr attr;
	int ret;

	mq = queue->mq;
	mq_getattr(mq, &attr);
	//printf("%s mq = %d, mq_curmsgs = %lu, mq_maxmsg = %lu\n",
	//		__FUNCTION__, mq, attr.mq_curmsgs, attr.mq_maxmsg);
	if (attr.mq_maxmsg == attr.mq_curmsgs)
	{
	//	printf("%s fail mq = %d 1\n", __FUNCTION__, mq);
		return OS_QUEUE_SEND_FAILED;
	}
	ret = mq_send (mq, pData, queue->element_size, 0);

	if (0 != ret)
	{
//		printf("%s fail mq = %d 2\n", __FUNCTION__, mq);
		return OS_QUEUE_SEND_FAILED;
	}

	//printf("%s success mq = %d ++++\n", __FUNCTION__, mq);
	return OS_QUEUE_SEND_SUCCESS;
}


uint8_t os_queue_send_infinite_wait(os_queue_t queue,const  void * pData  )
{
	uint8_t retVal;
	mqd_t  mq;
	int ret;

	mq = queue->mq;


	ret = mq_send (mq, pData, queue->element_size, 0);

	if (0 != ret)
	{
		return OS_QUEUE_SEND_FAILED;
	}

	return OS_QUEUE_SEND_SUCCESS;
}


uint8_t os_queue_receive_with_timeout(
				os_queue_t queue, void *pData, int timeout_msec)
{
	uint8_t retVal;
	mqd_t  mq;
	int ret;
	struct timespec abs_timeout;
	time_t sec;

	mq = queue->mq;
	clock_gettime(CLOCK_REALTIME, &abs_timeout);
//		printf("%s sec = %ul\n", __FUNCTION__, abs_timeout.tv_sec);
//		//printf("%s timeout_msec = %ul\n", __FUNCTION__, abs_timeout.tv_sec);
	if (1000 <= timeout_msec)
	{
		sec = timeout_msec / 1000;
		abs_timeout.tv_sec += sec;
		timeout_msec -= (1000 * sec);
	}
	abs_timeout.tv_nsec += (1000000 * timeout_msec );

//		printf("%s sec = %ul ++++\n", __FUNCTION__, abs_timeout.tv_sec);

	ret = mq_timedreceive(mq, pData, queue->element_size, NULL, &abs_timeout);

	if (-1 == ret)
	{
		return OS_QUEUE_RECEIVE_FAILED;
	}

	return OS_QUEUE_RECEIVE_SUCCESS;
}


uint8_t os_queue_receive_infinite_wait(os_queue_t queue, void *pData)
{
	uint8_t retVal;
	mqd_t  mq;
	int ret;
	unsigned int msg_prio = 15;

	mq = queue->mq;

	ret = mq_receive(mq, pData, queue->element_size, &msg_prio);


	if (-1 == ret)
	{
		return OS_QUEUE_RECEIVE_FAILED;
	}

	return OS_QUEUE_RECEIVE_SUCCESS;
}


os_mutex_t os_create_mutex(void)
{
	pthread_mutex_t *new_mutex;

	new_mutex =
			(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));

	if (NULL == new_mutex)
	{
		printf("cannot allocate memory for mutex\n");
		return NULL;
	}

	if (pthread_mutex_init(new_mutex, NULL) != 0)
	{
		printf("\n mutex init has failed\n");
		return NULL;
	}
	return new_mutex;
}


void os_start(void)
{

}





void os_init(void)
{

}

#ifdef CONFIG_TEST_TASK_STACK



#endif
