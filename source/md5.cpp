#include "md5.h"
#include <openssl/md5.h>


std::string md5(Blob * data)
{
    return md5(data->getData(), data->getLength());
}

std::string md5(const char * data, unsigned int length)
{
    char * hash = (char *)MD5((const unsigned char *)data, length, NULL);
    return std::string(hash, length);
}
