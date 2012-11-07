/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include <common/Error.h>
#include <common/uuid.h>
#include <common/strutil.h>
#include <common/HTTP/Error.h>
#include <common/coding/Encoder.h>
#include <common/MimeType.h>

using std::string;
using std::endl;
using std::stringstream;
using database::library::Library;
using coding::Encoder;


/** Creates a new connection object that will handle communication on the given
 * socket for the given server object. */
database::Connection::Connection(Socket *socket, Server *server) : HTTP::Connection(socket), server(server)
{
}

/** @inheritDoc */
void database::Connection::run()
{
	HTTP::Connection::run();
	server->removeConnection(this);
}

/** Handles requests sent by the connected client. */
void database::Connection::received(HTTP::Request *request)
{
	//Disassemble the request path to find out what is being requested and in what format.
	size_t lastSlash = request->path.find_last_of('/');
	size_t lastDot   = request->path.find_last_of('.');
	string path, suffix;
	if (lastDot != string::npos && (lastSlash == string::npos || lastSlash < lastDot)) {
		path   = request->path.substr(0, lastDot);
		suffix = request->path.substr(lastDot+1);
	} else {
		path = request->path;
		suffix = "";
	}
	log() << "serving " << path;
	if (!suffix.empty()) std::cout << " (as " << suffix << ")";
	std::cout << ", headers:" << endl << request->headers.toString();
	MimeType responseType = MimeType::makeWithSuffix(suffix);

	//Returns a list of song IDs stored in the database.
	if (path == "/songs" && request->type == HTTP::Request::kGET) {
		Encoder *encoder = Encoder::makeForSuffix(suffix);
		const Library::Songs &e = server->library->getSongs();
		for (Library::Songs::const_iterator it = e.begin(); it != e.end(); it++) {
			encoder->add((*it)->getID());
		}
		encoder->finalize();

		HTTP::Response r;
		r.headers.add("Content-Type", responseType.getName());
		r.content = encoder->getOutputString();
		r.finalize();
		write(r);
		close();
		return;
	}

	//Process metadata requests.
	if (strutil::consumePrefix(path, "/metadata/")) {
		library::Song *song = server->library->getSong(path);
		if (!song) {
			throw new HTTP::NotFoundError("No song with ID " + path + " exists.");
		}

		Encoder *encoder = Encoder::makeForSuffix(suffix);
		song->getMetadata()->encode(encoder);
		encoder->finalize();

		HTTP::Response r;
		r.headers.add("Content-Type", responseType.getName());
		r.content = encoder->getOutputString();
		r.finalize();
		write(r);
		close();
		return;
	}


	if (path == "/add") {
		library::Song *song = server->library->addMedia(Blob(request->content.c_str(), request->content.length()));
		if (!song) {
			throw new GenericError("Library didn't return a song upon addMedia().");
		}

		HTTP::Response r;
		r.content = song->getID();
		r.finalize();
		write(r);
		close();
		return;
	}
	else if (path.find("/download/") == 0) {
		string id = path.substr(10); //skip the /download/ part
		library::Song *song = server->library->getSong(id);
		if (!song) {
			throw new HTTP::NotFoundError(string("Song ") + id + " does not exist.", request);
		}
		Blob blob = song->loadMainFormat();

		HTTP::Response r;
		r.content.assign((const char*)blob.buffer, blob.length);
		r.finalize();
		write(r);
		close();
		return;
	}
	else if (path == "/info") {
		coding::Encoder *encoder = coding::Encoder::makeForSuffix(suffix);
		encoder->add("1.0.0 beta", "version");
		encoder->pushArray("names");
		encoder->add("hello");
		encoder->add("world");
		encoder->add("whadup?");
		encoder->popArray();
		encoder->add(235, "age");
		encoder->pushObject("structure");
		encoder->add("fred", "name");
		encoder->add("debug", "test");
		encoder->add(true, "someBool");
		encoder->add((float)34.2, "aFloat");
		encoder->popObject();
		encoder->finalize();

		HTTP::Response r;
		r.content = encoder->getOutputString();
		r.finalize();
		write(r);
		close();
		return;
	}

	throw new HTTP::NotFoundError(string("Requested object ") + path + " not found.", request);
}