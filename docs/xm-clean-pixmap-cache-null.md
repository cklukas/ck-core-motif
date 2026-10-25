# Xm pixmap-cache cleanup with uninitialized tables

`_XmCleanPixmapCache` now skips absent GC and pixmap hash tables. Cleanup is
therefore safe after lazy cache initialization fails, while initialized tables
continue to use the existing cleanup callbacks.

The source-level regression is
`tests/automated/check-xm-clean-pixmap-cache-null.py`.
