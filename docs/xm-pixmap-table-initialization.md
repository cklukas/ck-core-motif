# Xm pixmap-table initialization

`InitializePixmapSets` now allocates both pixmap hash tables before publishing
their globals. If the second allocation fails, the first table is freed and the
cache remains uninitialized and retryable instead of exposing a half-initialized
pair.

Successful initialization is unchanged. The source-level regression is
`tests/automated/check-xm-pixmap-table-initialization.py`.
