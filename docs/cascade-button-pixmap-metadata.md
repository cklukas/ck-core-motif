# CascadeButton pixmap metadata

`DrawCascade` now checks the result of `XmeGetPixmapData` before using the
returned depth.  If a pixmap is unavailable or its cache metadata cannot be
resolved, the cascade glyph draw is skipped safely instead of reading an
uninitialized depth value.

Successful metadata lookups retain the existing depth-matched `XCopyArea` or
bitmap `XCopyPlane` behavior.  The focused regression is
`tests/automated/check-cascade-pixmap-metadata.py`.
