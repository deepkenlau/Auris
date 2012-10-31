/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Player.h"
#include "../common/Socket.h"
#include "../common/Thread.h"
#include <common/HTTP/Request.h>
#include <common/HTTP/Response.h>
#include <common/Error.h>
#include <common/uuid.h>
#include <typeinfo>
#include "../common/Blob.h"
#include "../database/library/Error.h"
#include <ao/ao.h>

extern "C" {
	#define __STDC_CONSTANT_MACROS
	#define AVMEDIA_TYPE_AUDIO CODEC_TYPE_AUDIO
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/dict.h>
}

using player::Connection;
using std::string;
using std::endl;
using std::stringstream;

#define clog std::cout << "[connection " << getClientName() << "] "

//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}

/** Creates a new connection object that will handle communication on the given
 * socket for the given server object. */
Connection::Connection(Socket *socket, Player* player)
{
	this->socket = socket;
	this->player = player;
}

/** Main function of the connection thread. Simply calls the connection's run
 * function. */
static void* connectionThread(void *param)
{
	Connection *c = (Connection*)param;
	try {
		c->run();
	} catch (Error *e) {
		std::cerr << "*** uncaught connection exception: " << e->what() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "*** uncaught connection exception " << typeid(e).name() << ": " << e.what() << std::endl;
	}
	return NULL;
}

/** Starts the thread that handles this connection. */
void Connection::start()
{
	Thread::make(connectionThread, this);
}

void Connection::run()
{
	clog << "connected" << endl;
	try {
		//Enter the main loop which runs as long as the socket is connected.
		const int BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE];
		while (socket->isOpen()) {
			int num_read;

			//Wait for incoming data for 10ms.
			int bytesRead = 0;
			while (socket->poll(10) && socket->isOpen()) {
				num_read = socket->read(buffer, BUFFER_SIZE);
				inputBuffer.append(buffer, num_read);
				bytesRead += num_read;
			}

			//Call the reception handler if we've read any data.
			if (bytesRead) {
				this->received();
			}


			//Write data to the socket as long as there's data left in the output buffer.
			int write_offset = 0;
			do {
				outputBuffer_lock.lock();
				num_read = std::min<int>(BUFFER_SIZE, outputBuffer.length() - write_offset);
				outputBuffer.copy(buffer, num_read, write_offset);
				outputBuffer_lock.unlock();

				if (num_read > 0) {
					socket->write(buffer, num_read);
					write_offset += num_read;
				}
			} while (num_read > 0);

			if (write_offset > 0) {
				outputBuffer_lock.lock();
				outputBuffer.assign(outputBuffer, write_offset, outputBuffer.length() - write_offset);
				int length = outputBuffer.length();
				outputBuffer_lock.unlock();
				if (length == 0 && closeAfterWrite) {
					socket->close();
				}
			}
		}
	} catch (Error *e) {
		clog << "*** " << e->what() << endl;
		socket->write("500 Internal Server Error\r\n", 27);
	}

	//We're done, close the connection and remove it from the server.
	socket->close();
	clog << "closed" << endl;
	player->removeConnection(this);
}

/** Called whenever new data arrives in the input buffer. Called on the Connection's
 * thread, which makes it unnecessary to have a lock for the input buffer. */
void Connection::received()
{
	//Try to parse the HTTP request that was received.
	unsigned int consumed = 0;
	HTTP::Request *request = HTTP::Request::fromString(inputBuffer, &consumed);
	if (consumed > 0) {
		inputBuffer.assign(inputBuffer, consumed, inputBuffer.length() - consumed);
	}
	if (!request) return;
	clog << "processing request" << endl;

	clog << request->path << endl;
	if(request->path.at(0) != '/')
	{
		clog << "invalid request" << endl;
		HTTP::Response r;
		r.statusCode = 404;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
		return;
	}
	int i,j;
	for(i = 1; i < request->path.length() &&
		request->path.at(i) != '/'; i++)
	{}
	if (i == request->path.length())
	{
		clog << "invalid request" << endl;
		HTTP::Response r;
		r.statusCode = 400;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
		return;		
	}
	std::string command = request->path.substr(0,i);
	clog << command << endl;
	if (command == "/play")
	{
		for(j = i+1; j < request->path.length() &&
			request->path.at(j) != '/'; j++)
		{}
		clog << "j= " << j << endl << "length= " <<
			request->path.length() << endl;
		if (j == request->path.length())
		{
			clog << "invalid request" << endl;
			HTTP::Response r;
			r.statusCode = 400;
			r.statusText = string("Requested object ") + request->path + " not found.";
			r.finalize();
			write(r);
			close();
			return;		
		}
		std::string host = request->path.substr(i+1,j-(i+1));
		std::string uuid = request->path.substr(j+1);
		clog << "Playing: " << uuid << endl << "from: " << host << endl;
		//play
		//create new socket
		Socket * socket = Socket::makeConnected(host, 8080);
		//send request
		HTTP::Request request;
		request.type = HTTP::Request::kGET;
		stringstream str;
		str << "/download/";
		str << uuid;
		request.path = str.str();
		std::string data = request.toString();
		socket->write(data.c_str(), data.length());

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
		clog << response->content.substr(0,20) << endl;


		//Wrap the blob data in a stringstream object.
		std::istringstream stream(response->content);

		//Create a new AVIOContext for reading data from an istream instead of a file.
		unsigned char *buffer_3 = (unsigned char *)av_malloc(8192);
		AVIOContext *ioctx = avio_alloc_context(buffer_3, 8192, 0, (void*)&stream, &readistream, NULL, NULL);

		//Create a new AVFormatContext that uses the IO context created above.
		AVFormatContext* ctx = avformat_alloc_context();
		ctx->pb = ioctx;

		//Try to open the file.
		if (avformat_open_input(&ctx, "dummyFilename", NULL, NULL))
			throw new GenericError("Unable to avformat_open_input with the new media file.");

		if (av_find_stream_info(ctx) < 0)
        	throw new GenericError("Unable to av_find_stream_info.");

        int stream_id = -1;
	    // To find the first audio stream. This process may not be necessary
	    // if you can gurarantee that the ctx contains only the desired
	    // audio stream
	    int i;
	    for (i = 0; i < ctx->nb_streams; i++) {
	        if (ctx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
	            stream_id = i;
	            break;
	        }
	    }
	 
	    if (stream_id == -1) {
	        throw new GenericError("Could not find an audio stream");
	    }

	    //Extract some metadata.
	    AVDictionary* metadata = ctx->metadata;
		AVCodecContext* codec_context = ctx->streams[stream_id]->codec;
		AVCodec* codec = avcodec_find_decoder(codec_context->codec_id);
		if (!avcodec_open(codec_context, codec) < 0)
			throw new GenericError("Unable to avcodec_find_decoder.");

		//initialize libao for playback
		ao_initialize();
		int driver = ao_default_driver_id();

		//configure format
		ao_sample_format sample_format;
		sample_format.bits = 16;
		sample_format.channels = 2;
	    sample_format.rate = 44100;
	    sample_format.byte_format = AO_FMT_NATIVE;
	    sample_format.matrix = 0;
	    ao_device* device = ao_open_live(driver, &sample_format, NULL);
	    AVPacket packet;
	    int buffer_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
	    int8_t buffer_2[AVCODEC_MAX_AUDIO_FRAME_SIZE];

		while (1)
		{
	        // Read one packet into `packet`
	        if (av_read_frame(ctx, &packet) < 0) {
	            break;  // End of stream. Done decoding.
	        }
	 
	        // Decodes from `packet` into the buffer
	        if (avcodec_decode_audio3(codec_context, (int16_t*)buffer_2, &buffer_size, &packet) < 1) {
	            break;  // Error in decoding
	        }
	 
	        // Send the buffer contents to the audio device
	        ao_play(device, (char*)buffer_2, buffer_size);
	    }
	 
	    av_close_input_file(ctx);
	    ao_shutdown();

	}
	else {
		clog << "unable to serve requested object " << request->path << endl;
		HTTP::Response r;
		r.statusCode = 404;
		r.statusText = string("Requested object ") + request->path + " not found.";
		r.finalize();
		write(r);
		close();
	}
}

/** Stores the given data in the output buffer to be sent. Thread-safe. */
void Connection::write(const char *data, unsigned int length)
{
	outputBuffer_lock.lock();
	outputBuffer.append(data, length);
	outputBuffer_lock.unlock();
}

void Connection::write(HTTP::Response &r)
{
	write(r.toString());
}
void Connection::write(HTTP::Request &r)
{
	write(r.toString());
}
void Connection::write(const std::string &s)
{
	write(s.c_str(), s.length());
}

/** Marks the connection as to be closed as soon as the last byte of the output
 * buffer is sent. */
void Connection::close()
{
	closeAfterWrite = true;
}

/** Returns a string to identify the connected client. This might be the client's
 * IP address, the client's name if logged in, or the like. */
const std::string& Connection::getClientName()
{
	return socket->getRemoteAddress();

}