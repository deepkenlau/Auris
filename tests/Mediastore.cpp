#include <database/media/Store.h>
#include <iostream>

using namespace std;
using database::media::Store;

int main(void)
{
	Path path;
	Store store(path);
	/*char * str = "asdfg\n";
	Blob blob(str, 7);
	store.persist(blob, "bubber.txt", "mp3 bitches!\n");

	cout << (char*)store.load("bubber.txt", "mp3 bitches!\n").buffer << endl;*/
	cout << store.getMainFormat("bubber.txt") << endl;
	return 0;
}