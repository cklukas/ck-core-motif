# Xm scaled-pixmap cache-entry allocation

`_XmGetScaledPixmap` now checks allocation of the cache record, its color
metadata, and its copied image name before dereferencing or publishing the
entry. Failure cleanup restores any temporary XImage conversion, releases
allocated image pixels, and frees partial cache state while preserving the
existing process-lock ownership.

The source-level regression is
`tests/automated/check-xm-scaled-pixmap-entry.py`.
