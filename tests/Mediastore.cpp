#include <database/media/Store.h>
#include <iostream>
#include <set>

using namespace std;
using database::media::Store;

int main(void)
{	
	Store store("debug_media");
	char * str = "asdfg\n";
	Blob blob(str, 7);
	store.persist(blob, "bubber.txt", "mp3 bitches!");
	store.persist(blob, "haha.txt", "wave, u jelly?");	

	store.persist(blob, "gusta.txt", "wave, u jelly?");

	cout << (char*)store.load("bubber.txt", "mp3 bitches!").buffer << endl;

cout << "arrived here" << endl;


	set<string> formats = store.getFormats();
	for(set<string>::iterator it = formats.begin(); it != formats.end(); it++)
	{
		cout << *it << endl;
	}
	return 0;
}