#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


int main(int argc, char * argv[])
{
	//Initialize the library.
	av_register_all();
	
	//Open the audio file.
	AVFormatContext * formatContext;
	const char * filename = "audio/duelo.mp3";
	if (av_open_input_file(&formatContext, filename, NULL, 0, NULL)) {
		std::cerr << "unable to open file " << filename << std::endl;
		return 1;
	}
	
	//Retrieve the stream information.
	if (av_find_stream_info(formatContext) < 0) {
		std::cerr << "unable to find stream info" << std::endl;
		return 1;
	}
	
	//Dump information to the console.
	dump_format(formatContext, 0, filename, false);
	
	return 0;
}
