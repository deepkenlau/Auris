#include "response.h"


Response::Response(ResponseHandler * handler) : handler(handler)
{
    finished = false;
    pthread_mutex_init(&mutex, NULL);
}

Response::~Response()
{
}

void Response::finish()
{
    pthread_mutex_lock(&mutex);
    finished = true;
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
