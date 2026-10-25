# Deferred pixmap tag failures

When `Urm__CW_CreateArglist` finishes a deferred icon or bitmap argument, it
must decompress the resource tag before appending the argument. A failed tag
lookup previously left the name slot undefined and still increased the output
count. The finalization path now reports the lookup failure, releases the
pixmap it just created, and skips that argument, preserving the remaining
argument list and preventing a leaked server resource.

The source-level regression check is
`tests/automated/check-mrm-pixmap-tag-failure.py`.
