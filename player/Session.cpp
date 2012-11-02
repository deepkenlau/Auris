/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Session.h"
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
using std::endl;
using std::stringstream;

#define clog std::cout << "[session " << this->id << "] "


//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}

void Session::play(std::string host, std::string uuid)
{
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