#pragma once

class Blob 
{
private:
    char * data;
    unsigned long length;
public:
    Blob(char * data, unsigned long length){this->data = data; this->length = length;}
    ~Blob(void) {delete data;}
    
    char * getData() const { return data; }
    unsigned long getLength() const { return length; }
};
