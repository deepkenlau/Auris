/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Output.h"
#include "../common/Socket.h"
#include "../common/Thread.h"
#include <common/HTTP/Request.h>
#include <common/HTTP/Response.h>
#include <common/Error.h>
#include <common/MimeType.h>
#include <typeinfo>
#include "../common/Blob.h"
#include "../database/library/Error.h"
#include <ao/ao.h>
#include <iostream>
#include <complex>
#include <unistd.h>
#include <common/coding/Decoder.h>
#include <list>
#include <common/HTTP/Error.h>

extern "C" {
	#define __STDC_CONSTANT_MACROS
	#define AVMEDIA_TYPE_AUDIO CODEC_TYPE_AUDIO
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/dict.h>
}

using player::Output;
using coding::Decoder;

//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}

static void * outputThread(void *param)
{
	Output * o = (Output *)param;
	try
	{
		o->run();
	} catch (Error *e)
	{
		std::cerr << "*** uncaught output exception" << e->what() << std::endl;
	} catch (std::exception &e)
	{
		std::cerr << "*** uncaught output exception" << typeid(e).name()
			<< ": " << e.what() << std::endl;
	}
	return NULL;
}

void Output::start()
{
	Thread::make(outputThread, this);
}

void Output::run()
{
//debug
std::cout << "started run" << std::endl;
	while (true)
	{
		usleep(5000);

		while (playing_list)
		{

		//debug
		std::cout << "arrived at playing_list" << std::endl;	
			//fetch song, according to currentSong
			std::string url;
			url = songList[currentSong];
			size_t pos = url.find("://", 0);
			if (pos == std::string::npos
				|| url.substr(0, pos) != "http")
				throw new GenericError("Protocol is not HTTP.");

			size_t pos2 = url.find("/", pos+3);
			if (pos2 == std::string::npos)
				throw new GenericError("Invalid host.");
			std::string host_port = url.substr(pos+3, pos2-(pos+3));
			size_t pos3 = host_port.find(":", 0);
			int port = 8080;
			std::string host = host_port;
			if (pos3 != std::string::npos)
			{
				host = host_port.substr(0, pos3);
				port = atoi((char*)host_port.substr(pos3+1).c_str());	
			}

			HTTP::Request dbRequest;
			dbRequest.type = HTTP::Request::kGET;
			dbRequest.path = url.substr(pos2);
			dbRequest.headers.add("Host", host);
			dbRequest.headers.add("Connection", "close");

		//debug
		std::cout << dbRequest.path << std::endl;
		std::cout << host;
		std::cout << ":" << port << std::endl;


			Socket * socket = Socket::makeConnected(host, port);
			std::string dbRequest_data = dbRequest.toString();	
			socket->write(dbRequest_data.c_str(), dbRequest_data.length());
			
			//Enter the main loop which runs as long as the socket is connected.
			const int BUFFER_SIZE = 1024;
			char buffer[BUFFER_SIZE];	
			std::string inputBuffer;
			while (socket->isOpen())
			{
				int num_read = socket->read(buffer, BUFFER_SIZE);
				inputBuffer.append(buffer, num_read);
			}
			HTTP::Response * response = HTTP::Response::fromString(inputBuffer);
			int songSize = response->content.length();


			lock.lock();
			playing_song = true;
			lock.unlock();
		//debug
		std::cout << "start playing" << std::endl;
			while (playing_song)
			{
				while (paused) usleep(5000);
				lock.lock();
				if (currentPosition >= songSize)
				{
					playing_song = false;
					currentPosition = 0;
					currentSong++;
					lock.unlock();
					break;
				}
				
				std::cout << response->content.at(currentPosition);
				currentPosition++;
				lock.unlock();
			}


			//TODO: UPDATE LIST
		}
	}
}

void Output::source(HTTP::Request *request)
{
	MimeType mimeType = MimeType::makeWithName(request->headers.get("Content-Type"));
	Decoder * decoder = Decoder::make(request->content, mimeType.getName());
	Decoder::Object * object;
	decoder->getRootObject(object);
	std::string url;
	if (!object->getValue("url", url))
		throw new HTTP::BadRequestError("Root Object is not an URL");
	size_t pos = url.find("://", 0);
	if (pos == std::string::npos
		|| url.substr(0, pos) != "http")
		throw new HTTP::BadRequestError("Protocol is not HTTP.");

	size_t pos2 = url.find("/", pos+3);
	if (pos2 == std::string::npos)
		throw new HTTP::BadRequestError("Invalid host.");
	std::string host_port = url.substr(pos+3, pos2-(pos+3));
	size_t pos3 = host_port.find(":", 0);
	int port = 80;
	std::string host = host_port;
	if (pos3 != std::string::npos)
	{
		host = host_port.substr(0, pos3);
		port = atoi((char*)host_port.substr(pos3+1).c_str());	
	}
	
	//fetch playlist
	HTTP::Request dbRequest;
	dbRequest.type = HTTP::Request::kGET;
	dbRequest.path = url.substr(pos2);
	dbRequest.headers.add("Host", host);
	dbRequest.headers.add("Connection", "close");

	Socket * socket = Socket::makeConnected(host, port);
	std::string dbRequest_data = dbRequest.toString();	
	socket->write(dbRequest_data.c_str(), dbRequest_data.length());
	
	//Enter the main loop which runs as long as the socket is connected.
	const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];	
	std::string inputBuffer;
	while (socket->isOpen())
	{
		int num_read = socket->read(buffer, BUFFER_SIZE);
		inputBuffer.append(buffer, num_read);
	}
	HTTP::Response * response = HTTP::Response::fromString(inputBuffer);

	//ToDo: checkResponse

	//std::cout << "playlist: " << response->content << std::endl;
	
	if (response->headers.has("X-Playlist-ID"))
		list_id = atoi(response->headers.get("X-Playlist-ID").c_str());
	else list_id = 0;

	int i;
	for (i = url.length()-1; url.at(i) != '.' && i >= 0; i--) {}
	if (i == 0)
		throw new HTTP::BadRequestError("Invalid or no suffix.");
	std::string suffix = url.substr(i+1);
	mimeType = MimeType::makeWithSuffix(suffix);
	decoder = Decoder::make(response->content, mimeType.getName());
	Decoder::Array * array;
	if(!decoder->getRootArray(array))
		throw new GenericError("Invalid response from database");
	songList.erase(songList.begin(), songList.end());
	Decoder::Object * tmp_obj;
	while (array->getObject(tmp_obj))
	{
		int tmp_int;
		std::string tmp_str;
		if( tmp_obj->getValue("sequence", tmp_int) && tmp_obj->getValue("url", tmp_str) )
			songList[tmp_int] = tmp_str;
	}

	//debug
	std::cout << songList[1] << std::endl << songList[2] << std::endl;
}

void Output::play(HTTP::Request *request)
{	
	MimeType mimeType = MimeType::makeWithName(request->headers.get("Content-Type"));
	Decoder * decoder = Decoder::make(request->content, mimeType.getName());
	Decoder::Object * object;
	decoder->getRootObject(object);
	int sequence, position;
	if (!object->getValue("sequence", sequence))
		throw new HTTP::BadRequestError("Root Object has no sequence number");
	if (!object->getValue("position", position))
		throw new HTTP::BadRequestError("Root Object has no position");
	lock.lock();
	currentSong = sequence;
	currentPosition = position;
	playing_list = true;
	paused = false;
	lock.unlock();
}

void Output::pause()
{
	lock.lock();
	paused = true;
	lock.unlock();
}

void Output::resume()
{
	lock.lock();
	paused = false;
	lock.unlock();
}

void Output::stop()
{
	lock.lock();
	playing_song = false;
	playing_list = false;
	lock.unlock();
}

void Output::skip()
{
	lock.lock();
	currentPosition = 0;
	currentSong++;
	lock.unlock();
}

void Output::previous()
{
	lock.lock();
	currentPosition = 0;
	currentSong--;
	lock.unlock();
}

void Output::metadata()
{
	
}

int Output::getId() const
{
	return id;
}