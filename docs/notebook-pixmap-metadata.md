---
nav_exclude: true
---
# Notebook binding pixmap metadata

Notebook binding rendering now checks the result of `XmeGetPixmapData` before
using the returned pixmap depth.  If metadata cannot be resolved, the binding
draw is skipped safely instead of branching on an uninitialized depth value.

Successful lookups retain the existing stippled-versus-tiled GC behavior.  The
focused regression is `tests/automated/check-notebook-pixmap-metadata.py`.
