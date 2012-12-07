/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "ShufflePlaylist.h"
#include "library/Library.h"
#include "Server.h"
#include <cmath>

#define AURIS_CONSOLE_ENTITY "shuffle-playlist"
#define AURIS_CONSOLE_IDENTITY "#" << this->id
#include <common/console.h>

using database::ShufflePlaylist;
using database::Playlist;

const ShufflePlaylist::Range Playlist::kNilRange = (Playlist::Range){-1,-1};


ShufflePlaylist::ShufflePlaylist(Server *server, unsigned int id) : Playlist(server, id)
{
	min_id = 0;
	max_id = 0;
}

void ShufflePlaylist::encode(coding::Encoder *encoder, Range range)
{
	lock.lock();
	for (Songs::iterator is = songs.begin(); is != songs.end(); is++) {
		//if ((range.start < 0 || is->first >= range.start) && (range.length < 0 || is->first < range.start+range.length)) {
			clog << "encoding song " << is->first << " = " << is->second << std::endl;
			encoder->pushObject();
			encoder->add(is->first, "sequence");
			encoder->add(is->second, "url");
			encoder->popObject();
		//}
	}
	lock.unlock();
}

/*void ShufflePlaylist::extend()
{
	lock.lock();
	library::Library::Songs songs = server->library->getSongs();
	int max = songs.size();
	int i = rand() % max;
	
	lock.unlock();
}*/

void ShufflePlaylist::prepareRange(Range range)
{
	//Iterate through the list of songs, filling in the gaps where required.
	lock.lock();
	if (range.start < min_id) range.start = min_id;
	if (range.length < 0) range.length = std::max(max_id - range.start, 5);
	for (int i = 0; i < range.length; i++) {
		if (!songs.count(range.start+i)) {
			clog << "inserting song at " << range.start+i << std::endl;
			int new_id = range.start+i;

			//Make a list of all songs and their probability to be played.
			typedef std::map<library::Song*, double> Probabilities;
			Probabilities probs;

			//Find a new song to put in the playlist.
			library::Library::Songs librarySongs = server->library->getSongs();
			for (library::Library::Songs::iterator is = librarySongs.begin(); is != librarySongs.end(); is++) {
				//Find the sequence quality.
				double maxBadness = 0;
				double maxGoodness = 0.05;
				for (Songs::iterator it = songs.begin(); it != songs.end(); it++) {
					double distance = abs(it->first - new_id);
					double badness = 0;
					double goodness = 0;
					if (it->second == (*is)->getID()) {
						badness = 1; //make it impossible to play the same song with less than 10 other songs of spacing
					} else {
						Server::Quality quality = server->getQuality(it->second, (*is)->getID());
						double q = (quality.count <= 0 ? 0 : quality.quality / quality.count);
						if (q < 0) {
							badness = -q;
						} else {
							goodness = q;
						}
					}
					double distanceFactor = std::min(1.0, exp((10-distance)));
					badness *= distanceFactor;
					goodness *= distanceFactor;
					if (badness > maxBadness) maxBadness = badness;
					if (goodness > maxGoodness) maxGoodness = goodness;
				}
				probs[*is] = (1 - maxBadness) * maxGoodness;
			}

			//Convert the list of weights into probabilities.
			double total = 0;
			for (Probabilities::iterator it = probs.begin(); it != probs.end(); it++) {
				total += it->second;
			}
			for (Probabilities::iterator it = probs.begin(); it != probs.end(); it++) {
				it->second /= total;
				std::cout << it->first->getID() << " = " << it->second << std::endl;
			}

			//Choose one.
			double p = (double)(rand() % RAND_MAX) / RAND_MAX;
			clog << "p = " << p << std::endl;
			for (Probabilities::iterator it = probs.begin(); it != probs.end(); it++) {
				p -= it->second;
				if (p <= 0) {
					clog << "inserting " << it->first->getID() << " into playlist (whose probability was " << it->second << ")" << std::endl;
					songs[new_id] = it->first->getID();
					break;
				}
			}

			if (new_id > max_id) max_id = new_id;
			if (new_id < min_id) min_id = new_id;
		}
	}
	lock.unlock();
}