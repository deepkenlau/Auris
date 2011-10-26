#pragma once

class Blob 
{
	private:
		char* data;
		int length;
	public:
		Blob(char* data, length){this->data = data; this->length = length;}
		~Blob(void) {delete data;}
};
