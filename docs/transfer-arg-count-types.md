---
nav_exclude: true
---
# Transfer argument-count types

`XmeDragSource` and `XmeDropSink` assemble Xt argument lists whose public
count type is `Cardinal`. Their temporary counters now use that same type,
with explicit bounded allocations, avoiding signed narrowing and signed/unsigned
loop comparisons on 64-bit builds.

The focused regressions are
`tests/automated/check-transfer-arg-count-types.py` and
`tests/automated/check-drag-source-allocation.py`.
