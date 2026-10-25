# Arrow drawing size guards

`XmeDrawArrow` derives its internal raster size by subtracting two from the
requested width or height. Those parameters are public Motif `Dimension`
values, so widths or heights of one or two used to underflow before the later
positive-size check. The result could cause oversized temporary allocations
and unnecessary drawing work for a collapsed widget.

The helper now returns before that subtraction when either dimension is below
three pixels. Normal arrow geometry and the public API remain unchanged. The
regression `tests/automated/check-drarrow-size-guards.py` keeps the early guard
in place.
