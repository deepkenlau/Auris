/* Copyright © 2012 Fabian Schuiki, Sandro Sgier */
#include "Commit.h"
#include "Database.h"
#include <string>
#include <sstream>

using database::database::Commit;
using std::string;
using std::stringstream;


Commit::Commit(Database* db, const string &hash) : database(db), songs(this)
{
	this->hash = hash;
	this->base = NULL;
}

string Commit::persist()
{
	//Make the commit that we loaded our base commit.
	if (!hash.empty()) {
		base = new Commit(database, hash);
	}

	//Serialize the commit.
	stringstream str;
	if (base) {
		str << "base: " << base->getHash() << "\n";
	}
	str << "songs: " << songs.persist() << "\n";

	//Persist and keep the hash around.
	hash = getDatabase()->persistObject(str.str());
	return hash;
}

void Commit::load(const string &data)
{

}