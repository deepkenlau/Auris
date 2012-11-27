/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include <iostream>
extern "C" {
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
	#include <libavutil/dict.h>
}


int main(int argc, char const *argv[])
{
	AVFormatContext *fmt_ctx = NULL;
	AVCodecContext *dec_ctx = NULL;
	int audio_stream_index = -1;

	avcodec_register_all();
	av_register_all();
	std::cout << LIBAVFORMAT_IDENT << std::endl;

	if (argc != 2) {
		std::cerr << "usage: " << argv[0] << " AUDIO_FILE" << std::endl;
	}
	const char *path = argv[1];

	int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&fmt_ctx, path, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    /*if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }*/

    /* select the audio stream */
    /*ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a audio stream in the input file\n");
        return ret;
    }
    audio_stream_index = ret;
    dec_ctx = fmt_ctx->streams[audio_stream_index]->codec;*/

    /* init the audio decoder */
    /*if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open audio decoder\n");
        return ret;
    }*/

	av_dump_format(fmt_ctx, 0, NULL, 0);
	avformat_close_input(&fmt_ctx);

	return 0;
}