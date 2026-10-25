---
nav_exclude: true
---
# Xm stippled-pixmap failure handling

`XiCreateStippledPixmap` now rejects a null screen, returns failure when the
server cannot create the pixmap, and frees the pixmap if cache-entry
allocation fails. `XiReleaseStippledPixmap` ignores null screens and
unspecified pixmaps. Failed resources are never cached.

The source-level regression is `tests/automated/check-xm-stippled-pixmap-failure.py`.
