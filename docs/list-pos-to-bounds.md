---
nav_exclude: true
---
# `XmListPosToBounds` position bounds

`XmListPosToBounds` returns the rectangle for a visible list item.  Its X origin
subtracts highlight thickness from `BaseX`, and its Y origin adds the row offset
to `BaseY`.  Direct assignment to X11 `Position` could wrap these values for
large row offsets or transiently oversized margins.

Both returned origins now use the List bounded-position helper after widening
the arithmetic.  Valid ordinary coordinates are unchanged; extreme values
clamp to the representable X11 range.  The contract is protected by
`tests/automated/check-list-pos-to-bounds.py`.
