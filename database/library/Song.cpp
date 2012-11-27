/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Song.h"
#include "Library.h"
#include "Error.h"
extern "C" {
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
	#include <libavutil/dict.h>
}
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>

using database::library::Song;
using std::set;
using std::string;
using std::stringstream;


Song::Song(Library *l, std::string id) : library(l)
{
	this->id = id;
}

database::database::Song* Song::getMetadata() const
{
	database::Song *md = dynamic_cast<database::Song*>(library->getDatabase().getSongs().getEntity(id));
	if (!md) {
		throw new Error(string("No metadata available for song ") + id + ".");
	}
	return md;
}

set<string> Song::getFormats() const
{
	return library->getStore().getFormats();
}

Blob Song::loadMainFormat() const
{
	media::Store &store = library->getStore();
	return store.load(id, store.getMainFormat(id));
}



//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream *me = (std::istream*)opaque;
    std::cout << "reading " << buf_size << " into " << (void*)buf << std::endl;
    me->read((char*)buf, buf_size);
   	me->clear();
    return me->gcount();
}

static int64_t seekistream(void* opaque, int64_t offset, int whence)
{
	std::cout << "seek to " << offset << " (whence = " << whence << ")" << std::endl;
	whence &= ~AVSEEK_FORCE;
    std::istream *me = (std::istream*)opaque;
    std::ios_base::seekdir dir;
    if (whence == AVSEEK_SIZE) {
    	long long last = me->tellg();
    	me->seekg(0, std::ios::end);
    	int64_t length = me->tellg();
    	me->seekg(last, std::ios::beg);
    	me->clear();
    	std::cout << "  returning " << length << std::endl;
    	return length;
    }
    switch (whence) {
    	case SEEK_SET: dir = std::ios::beg; break;
    	case SEEK_CUR: dir = std::ios::cur; break;
    	case SEEK_END: dir = std::ios::end; offset = -offset; break;
    	default: throw new GenericError("Unknown whence code."); break;
    }
    me->seekg(offset, dir);
    	me->clear();
	return 0;
}

void Song::importMetadata(const Blob &blob)
{
	std::cout << LIBAVFORMAT_IDENT << std::endl;
	std::istringstream stream(string((const char*)blob.buffer, blob.length));

	//Create a new AVIOContext for reading data from an istream instead of a file.
	int buffer_size = 8192;
	unsigned char *buffer = (unsigned char *)av_malloc(buffer_size);
	std::cout << "allocated buffer " << (void*)buffer << std::endl;
	AVIOContext *ioctx = avio_alloc_context(buffer, buffer_size, 0, (void*)&stream, &readistream, NULL, &seekistream);

	//Create a new AVFormatContext that uses the IO context created above.
	AVFormatContext* ctx = avformat_alloc_context();
	ctx->pb = ioctx;

	//Try to open the file.
	if (avformat_open_input(&ctx, "", NULL, NULL) < 0)
		throw new GenericError("Unable to avformat_open_input with the new media file.");

	//Find the stream info.
	if (avformat_find_stream_info(ctx, NULL) < 0) {
		throw new GenericError("Unable to avformat_find_stream_info.");
	}

	//Open all the codecs.
	for (int i = 0; i < ctx->nb_streams; i++) {
		AVCodecContext *decoder = ctx->streams[i]->codec;
		decoder->codec = avcodec_find_decoder(decoder->codec_id);
		if (!decoder->codec) {
			throw new GenericError("Unable to find codec.");
		}
		if (avcodec_open2(decoder, decoder->codec, NULL) == 0) {
			std::cout << "opened codec " << decoder->codec->long_name << " (" << decoder->codec->name << ")" << std::endl;
		}
	}

	//Assemble the format information.
	/*stringstream format;
	format << ctx->iformat->name;
	std::cout << "name: " << ctx->iformat->name << std::endl;
	std::cout << "long name: " << ctx->iformat->long_name << std::endl;
	//std::cout << "extensions: " << ctx->iformat->extensions << std::endl;
	std::cout << "duration: " << ctx->duration << std::endl;
	std::cout << "bit rate: " << ctx->bit_rate << std::endl;
	for (int i = 0; i < ctx->nb_streams; i++) {
		AVStream *s = ctx->streams[i];
		std::cout << "stream #" << i << std::endl;
		std::cout << "  duration: " << s->duration << std::endl;
		std::cout << "  codec name: " << s->codec->codec_name << std::endl;
		std::cout << "  channels: " << s->codec->channels << std::endl;
		std::cout << "  sample rate: " << s->codec->sample_rate << std::endl;
	}*/
	av_dump_format(ctx, 0, NULL, 0);

	//importMetadata(ctx);
	//library->getDatabase().commit();
	avformat_free_context(ctx);
}

void Song::importMetadata(AVFormatContext *ctx)
{
	//Load the appropriate metadata into our database::Song representation.
	database::Song *md = getMetadata();
	AVDictionaryEntry *tag = NULL;
	if ((tag = av_dict_get(ctx->metadata, "title",  tag, 0))) md->title  = tag->value;
	if ((tag = av_dict_get(ctx->metadata, "artist", tag, 0))) md->artist = tag->value;
	if ((tag = av_dict_get(ctx->metadata, "album",  tag, 0))) md->album  = tag->value;

	//Dump the metadata for debugging purposes.
	av_dump_format(ctx, 0, NULL, 0);
	tag = NULL;
	printf("Raw Metadata:\n");
	while ((tag = av_dict_get(ctx->streams[0]->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
		printf("- %s = %s\n", tag->key, tag->value);
}