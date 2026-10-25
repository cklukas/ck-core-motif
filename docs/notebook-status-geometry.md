---
nav_exclude: true
---
# Notebook status-area geometry

Notebook status width now subtracts margins and scrollers in signed arithmetic
before clamping.  Collapsed pages therefore keep zero-sized status geometry
instead of wrapping to a large unsigned dimension; existing status/scroller
height selection is retained.

The source-level regression is
`tests/automated/check-notebook-status-geometry.py`.
