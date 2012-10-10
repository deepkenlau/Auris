/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#pragma once
#include <pthread.h>

/** Lightweight wrapper around pthreads. */
class Thread
{
	const pthread_t thread;
public:
	typedef void* (*Func) (void*);

	Thread(pthread_t thread) : thread(thread) {}

	void join() { pthread_join(thread, NULL); }

	static Thread self() { return Thread(pthread_self()); }
	static Thread make(Func func, void *param = NULL) {
		pthread_t t;
		pthread_create(&t, NULL, func, param);
		return Thread(t);
	}

	operator pthread_t () { return thread; }
};