Auris Project
=============

Use the following to test the server's PUT mechanism:
`curl -X PUT -d @test.txt http://localhost:12345/`

Loose Ends
----------
The following is a list of things that need to be done at this particular stage of the project:

- *ConnectionInterfaceHTTP*: disassemble multipart-form-data so the user may upload files via the webbrowser.
- Write the command portion of the server, especially the *Command Object* so the connection interfaces may wrap up whatever they receive in command objects.
