#include "glo_type.h"

#include <stdio.h>
#include <stdlib.h>

#include "thread.h"

pthread_mutex_t* mutex_init(void)
{
	pthread_mutex_t *mutex;

	mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (mutex == NULL){
		return NULL;
	}

	pthread_mutex_init(mutex, NULL);

	return mutex;
}

int mutexP(pthread_mutex_t *mutex)
{
	if (mutex == NULL){
		return -1;
	}

	return pthread_mutex_lock(mutex);
}

int mutexV(pthread_mutex_t *mutex)
{
	if (mutex == NULL){
		return -1;
	}

	return pthread_mutex_unlock(mutex);
}

void mutex_destroy(pthread_mutex_t *mutex)
{
	if (mutex != NULL){
		pthread_mutex_destroy(mutex);
		Free(mutex);
	}
}

pthread_t* thread_create(void *(*func)(void *), void *arg)
{
	int ret;
	pthread_t *thread;

	thread = (pthread_t *)malloc(sizeof(pthread_t));
	if (thread == NULL)
	{
		return NULL;
	}

	ret = pthread_create(thread, NULL, func, (void *)arg);
	if (ret != 0)
	{
		return NULL;
	}

	return thread;
}

int thread_join(pthread_t *thread)
{
	if (thread == NULL){
		return -1;
	}

	return pthread_join(*thread, NULL);
}

void thread_exit(void)
{
	pthread_exit(NULL);
	return;
}


/*
 * 功能：创建线�? */
pthread_t * CreateManageThread(void * arg, void *(*func)(void *))
{
	pthread_t *ManageThreadID = NULL;
	ManageThreadID = thread_create(func, NULL);
	if (ManageThreadID == NULL)
	{
		return NULL;
	}
	return ManageThreadID;
}


/*
 * 功能：回收线�? */
int StopManageLogicThread(pthread_t* ManageThreadID)
{
	if(ManageThreadID)
	{
		if(*ManageThreadID)
		{
			pthread_join(*ManageThreadID, NULL);
		}
		Free(ManageThreadID);
		ManageThreadID = NULL;
	}

	return 0;
}


