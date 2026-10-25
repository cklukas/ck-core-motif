---
nav_exclude: true
---
# FileSB full-path value allocation failure

File Selection Box search qualification combines the current directory and
typed filename for relative paths. The combined length is now checked against
both `size_t` and Xt's `Cardinal` width before allocation. If the directory,
length, or allocation is unavailable, the typed filename is retained as the
value and all owned temporary storage is released, allowing the search callback
to continue without dereferencing null storage.

The regression check is
`tests/automated/check-filesb-value-buffer-allocation.py`.
