---
nav_exclude: true
---
# Compound-text table conversion buffers

`XmCvtXmStringTableToTextProperty` now checks both the per-item pointer table
against host `size_t` and Xt's `Cardinal` byte limits, including explicit
sizes for the pointer table and final null-separated compound-text buffer. A failed allocation frees
already converted items, releases the table, and returns `BadAlloc` while the
application lock is balanced.

The regression check is
`tests/automated/check-txtpropcv-compound-table.py`.
