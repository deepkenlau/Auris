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

bool Response::isSuccessful()
{
	pthread_mutex_lock(&mutex);
	bool v = successful;
	pthread_mutex_unlock(&mutex);
	return v;
}

/** Initializes a new response object with the given response handler.*/
Response::Response(ResponseHandler * handler) : handler(handler)
{
    finished = false;
    expectedLength = 0;
    successful = true;
    pthread_mutex_init(&mutex, NULL);
}

Response::~Response()
{
	pthread_mutex_destroy(&mutex);
}

/** Marks the response as finished. This may cause the response handler
 * to close the communication channels that are used to send the re-
 * sponse. An object generating a response should mark that as finished
 * whenever it is done feeding data into it. */
void Response::finish()
{
    pthread_mutex_lock(&mutex);
    finished = true;
    pthread_mutex_unlock(&mutex);
}

/** Tells the response handler how many bytes of data it should expect
 * as a response. This is merely a hint and may be used to generate
 * things like the HTTP header and the like, and is not mandatory. */
void Response::setExpectedLength(unsigned long l)
{
	pthread_mutex_lock(&mutex);
	expectedLength = l;
	pthread_mutex_unlock(&mutex);
}

/** Indicates the flavour of the reponse, namely whether the command was
 * a success or not. */
void Response::setSuccessful(bool s)
{
	pthread_mutex_lock(&mutex);
	successful = s;
	pthread_mutex_unlock(&mutex);
}



/** Feeds the given data to the response object which relays it to the
 * ResponseHandler. */
void Response::write(const char * data, unsigned long length)
{
	if (!handler) return;
	handler->onResponseData(this, data, length);
}

/** Feeds the given data to the response object which relays it to the
 * ResponseHandler. */
void Response::write(Blob * data)
{
    if (!handler) return;
    handler->onResponseData(this, data->getData(), data->getLength());
}

/** Convenience wrapper for write(). */
Response & Response::operator << (Blob * data)
{
    write(data);
    return *this;
}

/** Convenience wrapper for write(). */
Response & Response::operator << (const std::string & s)
{
    write(s.c_str(), s.length());
    return *this;
}

/** Convenience wrapper for write(). */
Response & Response::operator << (const char * s)
{
    write(s, strlen(s));
    return *this;
}
