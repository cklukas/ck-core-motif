# Mrm buffer postconditions

`Idb__BM_GetRecord` and `Idb__BM_InitRecord` now verify that successful buffer
acquisition returned both a buffer object and record storage before reading or
initializing record headers.  Incomplete allocator results fail cleanly.

The source-level regression is
`tests/automated/check-mrm-buffer-postconditions.py`.
