---
nav_exclude: true
---
# FileSelectionBox search-list cleanup

File and directory searches now check result-table sizes against both host
`size_t` and Xt's `Cardinal` range.  A failed `XmStringGenerate` releases all
already-generated entries and the native filename list before returning, so a
partial list is never passed to `XmList`.  The normal file-search path also
clears its native list pointer after freeing it, preventing the shared exit
cleanup from freeing the same array twice.

The focused regression is
`tests/automated/check-filesb-search-list-cleanup.py`.
