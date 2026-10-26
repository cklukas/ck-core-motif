---
nav_exclude: true
---
# UIL debug-fragment flushing

UIL debug output may be assembled from several calls and the final call does
not always include a newline.  The listing writer keeps those fragments in a
bounded 132-byte buffer so long messages can be emitted in chunks.  Listing
cleanup now emits the remaining fragment and resets the buffer when a new
listing starts; debug text is therefore not silently lost or carried into a
later listing.

The focused source regression is
`tests/automated/check-uil-debug-flush.py`.
