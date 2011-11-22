#include "md5.h"
#include <openssl/md5.h>
#include <cstdio>


std::string md5(Blob * data)
{
    return md5(data->getData(), data->getLength());
}

std::string md5(const char * data, unsigned int length)
{
    unsigned char * hash = MD5((const unsigned char *)data, length, NULL);
    char sum[MD5_DIGEST_LENGTH*2 + 1];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        sprintf(sum + i*2, "%02x", hash[i]);
    return sum;
}
