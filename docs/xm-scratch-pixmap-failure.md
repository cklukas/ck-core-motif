---
nav_exclude: true
---
# Xm scratch-pixmap allocation failure handling

`_XmAllocScratchPixmap` now rejects missing cache tables, handles scratch-key
allocation failure, and frees the key when `XCreatePixmap` returns `None`.
Failed resources are never inserted into the in-use table.

The source-level regression is `tests/automated/check-xm-scratch-pixmap-failure.py`.
