---
nav_exclude: true
---

# Image-cache cleanup callbacks

GC and pixmap cache cleanup callbacks now return safely when the hash-table
entry, cleanup key, or cached screen is missing.  The same validation is used
by the optional debug printers.  This keeps display/print-shell teardown from
dereferencing incomplete cleanup state while preserving the normal removal
path for valid entries.

The regression check is
`tests/automated/check-image-cache-cleanup-callbacks.py`.
