---
nav_exclude: true
---

# Default render-table entry parsing

`XmeGetDefaultRenderTable` now processes every comma-separated entry in the
default font specification. Previously, the loop stopped as soon as the first
entry was loaded because it treated a non-empty render table as a stop
condition. Existing fallback behavior for entries that fail to load is
unchanged.

The focused regression check is
`tests/automated/check-default-render-table-entries.py`.
