---
nav_exclude: true
---
# Shared separator shadow span bounds

The shared `XmeDrawSeparator` implementation now computes its horizontal or
vertical shadow span in signed arithmetic and returns when the requested
margin consumes the span.  Callers with collapsed or unusually inset
separators therefore avoid wrapped dash lengths and coordinates.

The source-level regression is
`tests/automated/check-draw-separator-span-bounds.py`.
