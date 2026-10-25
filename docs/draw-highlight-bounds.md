# XmeDrawHighlight bounds

`XmeDrawHighlight` now clamps the requested highlight thickness to the
provided drawable dimensions and computes right/bottom coordinates in signed
arithmetic.  Oversized highlights therefore remain inside the drawable
instead of wrapping rectangle coordinates.

The source-level regression is `tests/automated/check-draw-highlight-bounds.py`.
