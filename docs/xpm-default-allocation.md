---
nav_exclude: true
---
# XPM default-color allocation

When XPM attributes provide default color strings, scanning duplicates those
strings into the output color record. A failed duplication now releases the
temporary pixel data and RGB-name table and returns `XpmNoMemory` instead of
leaving a partially initialized record.

The source contract is covered by
`tests/automated/check-xpm-default-allocation.py`.
