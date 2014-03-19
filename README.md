Welcome to the Auris project.


# To Do

Add a flag to the root objects (only *Index* at the moment) that indicates whether the object has been synchronized across devices. If this flag is not set, the object may be altered and resaved into the database, thus allowing for very lean databases during everyday use. If a database is to be synced, either a merge is performed which has this flag set anyway, or a copy of the root object is generated with this flag set.

Implement the synchronization algorithm which should communicate via an abstract channel that may be provided by the application that is using the library. E.g. a sync in a Mac OS X application may utilize Cocoa's classes to talk over the network, whereas a C++ program might use Boost's networking code. Naturally, the tools built by this project should offer at least synchronizations on the same filesystem as well as synchronizations via the network if the peer's address is known.

Add a *User* object type that captures subjective things about tracks, such as ratings and playlists. This is another root object that can be synchronized independently across devices.
