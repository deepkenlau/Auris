#include "response.h"


bool Response::isFinished()
{
	pthread_mutex_lock(&mutex);
	bool v = finished;
	pthread_mutex_unlock(&mutex);
	return v;
}

unsigned long Response::getExpectedLength()
{
	pthread_mutex_lock(&mutex);
	unsigned long v = expectedLength;
	pthread_mutex_unlock(&mutex);
	return v;
}

Response::Response(ResponseHandler * handler) : handler(handler)
{
    finished = false;
    pthread_mutex_init(&mutex, NULL);
}

Response::~Response()
{
	pthread_mutex_destroy(&mutex);
}

void Response::finish()
{
    pthread_mutex_lock(&mutex);
    finished = true;
    pthread_mutex_unlock(&mutex);
}

void Response::setExpectedLength(unsigned long l)
{
	pthread_mutex_lock(&mutex);
	expectedLength = l;
	pthread_mutex_unlock(&mutex);
}



void Response::write(const char * data, unsigned long length)
{
    handler->onResponseData(this, data, length);
}

void Response::write(Blob * data)
{
    handler->onResponseData(this, data->getData(), data->getLength());
}

Response & Response::operator << (Blob * data)
{
    write(data);
}

Response & Response::operator << (const std::string & s)
{
    write(s.c_str(), s.length());
}
