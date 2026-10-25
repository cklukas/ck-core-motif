# SegmentedControl layout bounds

When left-to-right segments exceeded the available width, the old layout
code computed `available - x` after `x` had already passed the edge.  Because
`Dimension` is unsigned, that underflow could configure a trailing button with
an enormous width.  The layout now clamps widths when the cursor reaches the
edge and keeps the cursor bounded while applying spacing.

`tests/automated/check-segmented-layout-bounds.py` keeps the bounded-span path
in place.
