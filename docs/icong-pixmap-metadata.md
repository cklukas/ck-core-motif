# IconG pixmap metadata guards

IconG now initializes pixmap depth before redisplay queries, so an unresolved
pixmap cannot select a copy operation using an undefined depth.  Background GC
creation only adds stipple or tile state for a positive resolved depth.  When
icon pixmaps change, their temporary width and height outputs are reset before
each query, preventing stale dimensions from being retained.

The focused regression is `tests/automated/check-icong-pixmap-metadata.py`.
