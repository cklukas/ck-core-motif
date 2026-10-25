---
nav_exclude: true
---
# FileSB full-path value allocation failure

File Selection Box search qualification combines the current directory and
typed filename for relative paths.  The directory text and combined-buffer
allocation are now checked.  If either is unavailable, the typed filename is
retained as the value and all owned temporary storage is released, allowing
the search callback to continue without dereferencing null storage.

The regression check is
`tests/automated/check-filesb-value-buffer-allocation.py`.
