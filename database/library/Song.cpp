/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Song.h"
#include "Library.h"
#include "Error.h"
extern "C" {
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
	#include <libavutil/dict.h>
}
#include <iostream>
#include <sstream>

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
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}

void Song::importMetadata(const Blob &blob)
{
	std::istringstream stream(string((const char*)blob.buffer, blob.length));

	//Create a new AVIOContext for reading data from an istream instead of a file.
	unsigned char *buffer = (unsigned char *)av_malloc(8192);
	AVIOContext *ioctx = avio_alloc_context(buffer, 8192, 0, (void*)&stream, &readistream, NULL, NULL);

	//Create a new AVFormatContext that uses the IO context created above.
	AVFormatContext* ctx = avformat_alloc_context();
	ctx->pb = ioctx;

	//Try to open the file.
	if (avformat_open_input(&ctx, "dummyFilename", NULL, NULL))
		throw new GenericError("Unable to avformat_open_input with the new media file.");

	//Assemble the format information.
	stringstream format;
	format << ctx->iformat->name;
	std::cout << "format is " << format.str() << std::endl;

	importMetadata(ctx);
	library->getDatabase().commit();
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