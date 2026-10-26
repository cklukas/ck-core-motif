---
nav_exclude: true
---
# Color conversion allocation safety

The black-and-white pixmap conversion duplicates a color-table entry before
inspecting it. Allocation failure now leaves the conversion path safe: the
duplicated string is checked before its first-byte test.

The source contract is covered by
`tests/automated/check-color-conversion-allocation.py`.
