---
nav_exclude: true
---
# Toggle indicator bounds

The shared `XmeDrawIndicator` implementation draws checkmarks and crosses
inside a widget rectangle. Its old scaling and endpoint arithmetic subtracted
unsigned margins from unsigned dimensions before checking whether any drawing
area remained. A very small indicator could therefore underflow to a large
scale or wrapped segment coordinates.

`DrTog.c` now performs the fit calculation in signed `int` locals and returns
without drawing when the margin consumes the available area. This preserves
the public API and normal indicator geometry while making undersized gadgets
safe. `tests/automated/check-drtog-indicator-bounds.py` covers both paths.
