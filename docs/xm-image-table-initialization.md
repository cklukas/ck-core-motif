---
nav_exclude: true
---
# Xm image-table initialization

`InitializeImageSet` now constructs the built-in image hash table privately and
publishes it only after every built-in `ImageData` entry has been allocated and
inserted. Concurrent callers serialize on the Motif process lock and reuse the
first completed table. A failed table or entry allocation releases all partial state and
leaves `image_set` null so a later call can retry. The shared hash-table
allocator now also releases its opaque table record when bucket allocation
fails, so callers only receive complete tables.

Successful built-in image lookup is unchanged. The source-level regression is
`tests/automated/check-xm-image-table-initialization.py`.
