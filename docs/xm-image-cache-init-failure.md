---
nav_exclude: true
---
# Xm image-cache initialization failure handling

`GetImage` lazily creates the image hash table. It now checks the result of
`InitializeImageSet()` before querying the table, returning `False` when table
allocation fails instead of passing a null table to `_XmGetHashEntry`.

Successful initialization and image lookup are unchanged. The source-level
regression is `tests/automated/check-xm-image-cache-init-failure.py`.
