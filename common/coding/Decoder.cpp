/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Decoder.h"


bool Decoder::Object::getValue(const std::string &key, int &v);
bool Decoder::Object::getValue(const std::string &key, double &v);
bool Decoder::Object::getValue(const std::string &key, bool &v);


bool Decoder::Object::getValue(int index, int &v);
bool Decoder::Object::getValue(int index, double &v);
bool Decoder::Object::getValue(int index, bool &v);