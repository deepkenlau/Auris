/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <pthread.h>

/** Wrapper around POSIX mutexes. */
class Mutex
{
	pthread_mutex_t mutex;

public:
	Mutex()  { mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER; pthread_mutex_init(&mutex, NULL); }
	~Mutex() { pthread_mutex_destroy(&mutex); }

	void lock()   { pthread_mutex_lock(&mutex); }
	void unlock() { pthread_mutex_unlock(&mutex); }
};