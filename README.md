Auris Project
=============

Use the following to test the server's PUT mechanism:
`curl --upload-file test.txt http://localhost:12345/`

Loose Ends
----------
The following is a list of things that need to be done at this particular stage of the project:

- *ConnectionInterfaceHTTP*: disassemble multipart-form-data so the user may upload files via the webbrowser.

- Think of a way how the commands are interpreted. At the moment, there's a `RawCommand` object that doesn't do any content-sensitive processing. Maybe the command portion should try to disassemble JSON, YAML, Confuse or whatever into a more meaningful structure and put that into a `Command` object. An additional `DataCommand` or whatever would also be quite helpful.

- Create an event system so that subsystems may inform other subsystems about essential changes that might require new computations. E.g. the storage subsystem would throw an event whenever a track is added or deleted, so the metadata subsystem may react accordingly and the aggregator may update its track indices accordingly.

Dependencies
------------
The Auris project depends on the following other projects and libraries:

- libuuid
- libconfig

And *will* probably depend on the following in the future:

- OpenAL
- libavcodec *from FFmpeg*
