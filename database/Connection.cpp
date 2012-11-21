/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Connection.h"
#include "Server.h"
#include <common/Error.h>
#include <common/uuid.h>
#include <common/strutil.h>
#include <common/HTTP/Error.h>
#include <common/coding/Encoder.h>
#include <common/coding/Decoder.h>
#include <common/MimeType.h>

using std::string;
using std::endl;
using std::stringstream;
using database::library::Library;
using coding::Encoder;
using coding::Decoder;


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
	MimeType requestType;
	if (request->headers.has("Content-Type"))
		requestType = MimeType::makeWithName(request->headers.get("Content-Type"));
	else
		requestType = responseType;

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
			throw new HTTP::NotFoundError("No song with ID " + path + " exists.", request);
		}

		if (request->type == HTTP::Request::kGET) {
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
		else if (request->type == HTTP::Request::kPUT) {
			Decoder *decoder = Decoder::make(request->content, requestType.getName());
			song->getMetadata()->decode(decoder->getRootObject());
			server->library->getDatabase().commit();

			HTTP::Response r;
			r.finalize();
			write(r);
			close();
			return;
		}

		throw new HTTP::MethodNotAllowedError("Method not allowed for metadata.", request);
	}

	//Process media requests.
	//  PUT /media
	//  GET /media/{id}
	//  GET /media/{id}/{format}
	if (strutil::consumePrefix(path, "/media")) {
		if (request->type == HTTP::Request::kGET) {
			//Extract the ID of the media file in question.
			if (path.length() > 1 && path[0] == '/') {
				size_t slash = path.find('/', 1);
				string id, format;
				if (slash != string::npos) {
					id = path.substr(1, slash-1);
					format = path.substr(slash+1);
				} else {
					id = path.substr(1);
				}

				//Fetch the song from the library.
				library::Song *song = server->library->getSong(id);
				if (!song) {
					throw new HTTP::NotFoundError(string("Song ") + id + " does not exist.", request);
				}

				//Either get the main format or the format specified.
				Blob blob;
				if (format.empty()) {
					blob = song->loadMainFormat();
				} else {
					//blob = song->loadFormat(format);
					throw new HTTP::NotFoundError("Media requests in specific format not yet supported.", request);
				}

				HTTP::Response r;
				r.content.assign((const char*)blob.buffer, blob.length);
				r.finalize();
				write(r);
				close();
				return;
			}
		}
		else if (request->type == HTTP::Request::kPUT) {
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
	}

	throw new HTTP::NotFoundError(string("Requested object ") + path + " not found.", request);
}