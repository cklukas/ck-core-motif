---
nav_exclude: true
---

# Image-cache cleanup callbacks

GC and pixmap cache cleanup callbacks now return safely when the hash-table
entry or cleanup key is missing.  This keeps display/print-shell teardown from
dereferencing incomplete cleanup state while preserving the normal removal
path for valid entries.

The regression check is
`tests/automated/check-image-cache-cleanup-callbacks.py`.
