---
nav_exclude: true
---
# Icon placement conversion guards

Icon placement conversion now validates placement dimensions, cell sizes, and
root-window coordinate arrays before doing division or indexing.  Invalid
state produces `(0, 0)` for the position conversion or `NO_ICON_PLACE` for the
reverse conversion.

`tests/automated/check-placement-conversion-guards.py` protects these bounds.
