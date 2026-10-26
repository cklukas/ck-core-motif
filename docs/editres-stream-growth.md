# Editres protocol-stream growth

Editres serializes widget inspection data into a growable protocol stream.
The byte-appending and reset paths now check capacity addition, use temporary
reallocation pointers, and return without publishing a partial stream when
allocation fails.  This avoids writing through a null pointer or wrapping the
header-inclusive size.
