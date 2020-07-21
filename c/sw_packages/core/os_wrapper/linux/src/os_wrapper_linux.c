/*
 * file : os_wrapper_linux.c
 *
 *
 *
 */


#include "_project.h"
#include "errors_api.h"

#ifdef CONFIG_TEST_TASK_STACK
	#define DEBUG
	#include "PRINTF_api.h"
#endif

#include "os_wrapper.h"

#include <errno.h>

#ifdef CONFIG_INCLUDE_HEARTBEAT
#include "heartbeat_api.h"
#endif

/**< @brief Nanoseconds per second. */
#define NANOSECONDS_PER_SECOND         ( 1000000000 )

/**< @brief Nanoseconds per millisecond. */
#define NANOSECONDS_PER_MILLISECOND    ( 1000000 )

/**< @brief Milliseconds per second. */
#define MILLISECONDS_PER_SECOND        ( 1000 )

typedef void *(*thread_func_t)(void*);

static struct dev_desc_t * l_timer_dev = NULL;
static struct dev_desc_t * l_heartbeat_dev = NULL;



static uint8_t timeout_to_timespec(uint32_t timeoutMs,
                                 struct timespec * output )
{
    struct timespec systemTime = { 0 };

    if( 0 != clock_gettime( CLOCK_REALTIME, &systemTime ) ) return 1;
\
	/* Add the nanoseconds value to the time. */
	systemTime.tv_nsec +=
			( long ) ( ( timeoutMs % MILLISECONDS_PER_SECOND ) *
					NANOSECONDS_PER_MILLISECOND );

	/* Check for overflow of nanoseconds value. */
	if( systemTime.tv_nsec >= NANOSECONDS_PER_SECOND )
	{
		systemTime.tv_nsec -= NANOSECONDS_PER_SECOND;
		systemTime.tv_sec++;
	}

	/* Add the seconds value to the timeout. */
	systemTime.tv_sec += ( time_t ) ( timeoutMs / MILLISECONDS_PER_SECOND );

	/* Set the output parameter. */
	*output = systemTime;

    return 0;
}

void *os_create_detached_task(
		char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, uint16_t stack_size_bytes, uint8_t priority)
{
	int err;
	pthread_t  thread_id;
    pthread_attr_t threadAttributes;

    err = 0;
    /* Set up thread attributes object. */
    err = pthread_attr_init( &threadAttributes );

    if( err != 0 )
    {
        printf("err=%d\n", err);
        CRITICAL_ERROR("cannot init thread attributes\n");
    }

    /* Set the new thread to detached. */
    err = pthread_attr_setdetachstate( &threadAttributes,
                                              PTHREAD_CREATE_DETACHED );
    if( err != 0 )
    {
        printf("err=%d\n", err);
        CRITICAL_ERROR("cannot set detach thread attribute\n");
    }

    thread_id = 0;
	err = pthread_create(&thread_id, &threadAttributes,
			(thread_func_t)taskFunction, taskFunctionParam);
	if (err != 0)
	{
		printf("\ncan't create thread :[%s]\n", strerror(err));
	}

	return (void*)thread_id;
}


void *os_create_task(char *taskName, void (*taskFunction)(void *apParam),
		void *taskFunctionParam, uint16_t stack_size_bytes, uint8_t priority)
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
	mode_t mode;
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

	snprintf(name, 16, "/queue%u", i);
	os_queue =
			(struct os_wrapper_queue *)malloc(sizeof(struct os_wrapper_queue));
	errors_api_check_if_malloc_succeed(os_queue);
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
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;// | S_IROTH |S_IWOTH;
	mq = mq_open(name, O_CREAT | O_RDWR , mode, &attr);
	if (-1 == mq)
	{
		printf("cannot open queue\n");
		return NULL;
	}

	os_queue->mq = mq;
	os_queue->element_size = size_of_elements;

	return os_queue;

}


uint8_t os_queue_send_without_wait(os_queue_t queue,const  void * pData  )
{
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
	mqd_t  mq;
	int ret;
	struct timespec abs_timeout;

	mq = queue->mq;
	clock_gettime(CLOCK_REALTIME, &abs_timeout);
//		printf("%s sec = %ul\n", __FUNCTION__, abs_timeout.tv_sec);
//		//printf("%s timeout_msec = %ul\n", __FUNCTION__, abs_timeout.tv_sec);
	if (1000 <= timeout_msec)
	{
		time_t sec;

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

	new_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	errors_api_check_if_malloc_succeed(new_mutex);

	if (pthread_mutex_init(new_mutex, NULL) != 0)
	{
		printf("\n mutex init has failed\n");
		return NULL;
	}
	return new_mutex;
}


os_mutex_t os_create_recursive_mutex(void)
{
	pthread_mutex_t *new_mutex;
    pthread_mutexattr_t mutexAttributes, * pMutexAttributes = NULL;
    int err = 0;

	new_mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	errors_api_check_if_malloc_succeed(new_mutex);

	err = pthread_mutexattr_init( &mutexAttributes );

    if( err != 0 )
    {
        printf("err=%d\n", err);
        CRITICAL_ERROR("cannot init thread attributes\n");
    }

    pMutexAttributes = &mutexAttributes;

    /* Set recursive mutex type. */
    err = pthread_mutexattr_settype( &mutexAttributes,
                                            PTHREAD_MUTEX_RECURSIVE );

    if( err != 0 )
    {
        printf("err=%d\n", err);
        CRITICAL_ERROR("cannot set thread attributes\n");
    }


	if (pthread_mutex_init(new_mutex, pMutexAttributes) != 0)
	{
		printf("\n mutex init has failed\n");
		return NULL;
	}
	return new_mutex;
}


os_semaphore_t os_create_semaphore(uint32_t initial_value)
{
	sem_t *sem;

	sem =(sem_t *)malloc(sizeof(sem_t));
	errors_api_check_if_malloc_succeed(sem);

    if( sem_init( sem, 0, ( unsigned int ) initial_value ) != 0 )
    {
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot init semaphore\n");
    }
    return sem;
}


uint32_t os_semaphore_get_count(os_semaphore_t sem)
{
    int count = 0;

    if ( sem_getvalue( sem, &count ) != 0 )
    {
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot get semaphore count\n");
    }

    return ( uint32_t ) count;

}


uint8_t os_semaphore_take_with_timeout(os_semaphore_t sem, uint32_t timeout_ms)
{
    struct timespec timeout = { 0 };

    if( 0 != timeout_to_timespec( timeout_ms, &timeout ) )
    {
    	CRITICAL_ERROR( "Invalid timeout." );
    	return 1;
    }

	if( sem_timedwait(sem, &timeout) != 0 )
	{
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot take timed semaphore\n");
        return 1;
	}

    return 0;
}


uint8_t os_semaphore_take_infinite_wait(os_semaphore_t sem)
{
	if( sem_wait(sem) != 0 )
	{
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot take semaphore\n");
        return 1;
	}

    return 0;
}


uint8_t os_semaphore_try_wait(os_semaphore_t sem)
{
	if( sem_trywait(sem) != 0 )
	{
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot take semaphore\n");
        return 1;
	}

    return 0;
}


uint8_t os_semaphore_give(os_semaphore_t sem)
{
    if( sem_post(sem) != 0 )
    {
        printf( "errno=%d. \n", errno );
        CRITICAL_ERROR("cannot give semaphore\n");
        return 1;
    }
    return 0;
}


void os_delete_mutex(os_mutex_t mutex)
{

    int err = pthread_mutex_destroy( mutex );

    if( err != 0 )
    {
        printf("errno=%d\n", errno);
        CRITICAL_ERROR("cannot delete mutex\n");
    }
    free(mutex);
}


void os_delete_semaphore(os_semaphore_t sem)
{
    if( sem_destroy(sem) != 0 )
    {
        printf("errno=%d\n", errno);
        CRITICAL_ERROR("cannot delete semaphore\n");
    }
}


void os_start(void)
{

}





void os_init(void)
{

}

#ifdef CONFIG_TEST_TASK_STACK



#endif
