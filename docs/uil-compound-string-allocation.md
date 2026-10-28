# UIL compound-string allocation failures

Mixed 8-bit/16-bit literals are assembled from a compound-string node and
component string nodes.  The lexer now checks each allocation before copying
or appending data, and localized-string length accumulation is checked before
signed arithmetic.  `sem_create_cstr` also returns cleanly when its semantic
node allocation fails, avoiding its own null dereference.
