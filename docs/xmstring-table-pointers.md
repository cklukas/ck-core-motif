---
nav_exclude: true
---
# XmString table component pointers

`XmStringToXmStringTable` now checks both component pointers before comparing
non-empty component data with `memcmp`.  Empty components retain their existing
matching behavior, while malformed non-empty components no longer cause a NULL
dereference.

`tests/automated/check-xmstring-table-pointers.py` protects both match loops.
