# Xm pixmap-set initialization

`InitializePixmapSets` now checks the published cache pointers while holding
the Motif process lock. A concurrent first caller reuses the tables published
by the first successful initializer instead of allocating a second pair or
overwriting global state. Partial allocation cleanup remains unchanged.

The source-level regression is
`tests/automated/check-xm-pixmap-set-initialization.py`.
