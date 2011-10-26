Auris Project
=============

Use the following to test the server's PUT mechanism:
`curl --upload-file test.txt http://localhost:12345/`

Loose Ends
----------
The following is a list of things that need to be done at this particular stage of the project:

- *ConnectionInterfaceHTTP*: disassemble multipart-form-data so the user may upload files via the webbrowser.
- Think of a way how the commands are interpreted. At the moment, there's a `RawCommand` object that doesn't do any content-sensitive processing. Maybe the command portion should try to disassemble JSON, YAML, Confuse or whatever into a more meaningful structure and put that into a `Command` object. An additional `DataCommand` or whatever would also be quite helpful.
- Think of a way how commands should be distributed to subsystems and whether they should be interpreted in the command subsystem already or later on in the individual subsystems.