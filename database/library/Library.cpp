#include <iostream>
#include <fstream>
#include <common/uuid.h>
#include "Library.h"
#include "Error.h"
extern "C" {
	#include <libavformat/avformat.h>
	#include <libavutil/dict.h>
}

using database::library::Library;
using database::library::Song;
using std::string;
using std::endl;
using std::stringstream;

#define clog std::cout << "[library " << this->path << "] "


Library::Library(Path p) : path(p), database(p), store(p)
{
}

/** Reads the metadata database and builds the internal list of songs. */
void Library::load()
{
	database.load();

	//Go through the entries in the metadata database and create a song for each.
	const database::Table::Entities &entities = database.getSongs().getEntities();
	songs_lock.lock();
	for (database::Table::Entities::const_iterator ie = entities.begin(); ie != entities.end(); ie++) {
		Song *song = new Song(this, (*ie)->getID());
		songs.insert(song);
	}
	clog << songs.size() << " songs loaded" << endl;
	songs_lock.unlock();
}

//Used by the AVIOContext to read data from an istream instead of a file.
static int readistream(void* opaque, uint8_t* buf, int buf_size)
{
    std::istream& me = *reinterpret_cast<std::istream*>(opaque);
    me.read(reinterpret_cast<char*>(buf), buf_size);
    return me.gcount();
}

/** Adds the given media file to the library. Extrcts the file's metadata and
 * fills the initial metadata entry with this information. Also extracts the
 * format information and persists the media file appropriately. */
Song* Library::addMedia(const Blob &blob)
{
	//Generate a UUID for this new media.
	string uuid = uuid::generate();
	clog << "adding media " << uuid << " of " << blob.length << " Bytes" << endl;

	//Wrap the blob data in a stringstream object.
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

	//Persist the data to the media store.
	store.persist(blob, uuid, format.str());
	store.setMainFormat(uuid, format.str());

	//Create the metadata entry.
	database::Song *md = database.getSongs().newEntity();
	md->setID(uuid);

	AVDictionaryEntry *tag = NULL;
	if ((tag = av_dict_get(ctx->metadata, "title",  tag, 0))) md->title  = tag->value;
	if ((tag = av_dict_get(ctx->metadata, "artist", tag, 0))) md->artist = tag->value;
	if ((tag = av_dict_get(ctx->metadata, "album",  tag, 0))) md->album  = tag->value;
	clog << "create metadata song " << md->describe() << endl;

	database.commit();

	//Dump the metadata for debugging purposes.
	tag = NULL;
	while ((tag = av_dict_get(ctx->metadata, "", tag, 0)))
		printf("%s=%s\n", tag->key, tag->value);

	//Clean up.
	avformat_free_context(ctx);

	//Create the song object for this media file.
	Song *song = new Song(this, uuid);
	songs_lock.lock();
	songs.insert(song);
	songs_lock.unlock();
	return song;
}

/** Returns the song with the given ID, or NULL if it doesn't exist. */
Song* Library::getSong(string id)
{
	Song* song = NULL;
	songs_lock.lock();
	for (Songs::const_iterator is = songs.begin(); is != songs.end(); is++) {
		if ((*is)->getID() == id) {
			song = *is;
			break;
		}
	}
	songs_lock.unlock();
	return song;
}

/** Returns a set of all songs in the library. */
Library::Songs Library::getSongs()
{
	songs_lock.lock();
	Library::Songs s = songs;
	songs_lock.unlock();
	return s;
}