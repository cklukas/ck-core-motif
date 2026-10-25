# IconG horizontal clip bounds

IconG now computes the main icon and detail clip widths in signed arithmetic,
clamping negative available space to zero before converting to `Dimension`.
Right-to-left clip origins are derived from the bounded width.  Collapsed
gadgets therefore avoid wrapped clip rectangles and accidental drawing across
the window.

The source-level regression is `tests/automated/check-icong-clip-bounds.py`.
