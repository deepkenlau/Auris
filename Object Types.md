Object Types in an Auris Database
=================================

## Index
A track listing, usually representing one music library. Contains a list of (id, hash) pairs that maps a track's persistent ID to the track object.

## Track
A single track. Mainly, this consists of the track's metadata such as title, artist, album, release date, genre, etc. In addition to this, the different audio data blobs for this track are listed as (hash, format, original_filename) triples.

Note that listing a track's audio blobs within the track metadata might not be the most efficient approach. If the user decides to add a 128bit MP3 version of all their music for mobile devices, every single track will be marked as changed. A more robust approach would be to have an *Audio* object type that lists the different blobs separately, whose hash would be added to the *Index* instead of the individual tracks.

## Blob
A chunk of binary data. Usually contains the audio data for a specific track.
