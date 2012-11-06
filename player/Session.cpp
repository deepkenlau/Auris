/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Session.h"
#include "Connection.h"
#include "../common/Socket.h"
#include "../common/Thread.h"
#include <common/HTTP/Request.h>
#include <common/HTTP/Response.h>
#include <common/Error.h>
#include <typeinfo>
#include "../common/Blob.h"
#include "../database/library/Error.h"
#include <ao/ao.h>
#include <iostream>
#include <complex>

extern "C" {
	#define __STDC_CONSTANT_MACROS
	#define AVMEDIA_TYPE_AUDIO CODEC_TYPE_AUDIO
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/dict.h>
}

using player::Session;
using player::Connection;
using std::endl;
using std::stringstream;
using std::string;

//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}


static void* sessionThread(void *param)
{
	Session * s = (Session *)param;
	try {
		s->run();
	} catch (Error *e) {
		std::cerr << "*** uncaught connection exception: " << e->what() << std::endl;
	} catch (std::exception &e) {
		std::cerr << "*** uncaught connection exception " << typeid(e).name() << ": " << e.what() << std::endl;
	}
	return NULL;
}

void Session::run()
{
	int i;
	for (i = 0; i < host.length()
		&& host.at(i) != '/'; i++) {}
	if (i == host.length())
		throw new GenericError("Invalid host.");

	std::string hostname = host.substr(0,i);
	std::string uuid = host.substr(i);
	//play
	//create new socket
	Socket * socket = Socket::makeConnected(hostname, 8080);
	//send request
	HTTP::Request request;
	request.type = HTTP::Request::kGET;
	stringstream str;
	str << "/download";
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

	    int buffer_size;
	    int8_t buffer_2[AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];

	    AVPacket packet;
	    packet.size = 0;
	    size_t offsetInData = 0;
	    bool eof = false;

		while (!eof) {
			//Read a packet.
			if (offsetInData >= packet.size) {
				av_free_packet(&packet);
				offsetInData = 0;
				while (true) {
					if (av_read_frame(ctx, &packet) < 0) {
						eof = true;
						break;
					}
					if (packet.stream_index == stream_id)
						break;
					av_free_packet(&packet);
				}
			}
			if (eof) break;

			//Feed the packet to libavcodec.
			AVPacket packetToSend;
			packetToSend.data = packet.data + offsetInData;
			packetToSend.size = packet.size - offsetInData;
			
			//Sending data to libavcodec
	   		buffer_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;
			int processedLength = avcodec_decode_audio3(codec_context, (int16_t*)buffer_2, &buffer_size, &packetToSend);
			if (processedLength < 0) {
				av_free_packet(&packet);
				throw new GenericError("Error while processing the data.");
			}
			offsetInData += processedLength;
	 
	        // Send the buffer contents to the audio device
	        ao_play(device, (char*)buffer_2, buffer_size);
	    }
	 
	    avformat_close_input(&ctx);
	    ao_shutdown();   
}


void Session::start()
{
	Thread::make(sessionThread, this);
}

int Session::getId() const
{
	return id;
}

void Session::play(std::string host)
{
	this->host = host;
}