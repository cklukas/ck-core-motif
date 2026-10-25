---
nav_exclude: true
---
# IconButton redisplay bounds

`XmIconButton` stores its core width and height as unsigned `Dimension`
values.  The redisplay path now checks each dimension before subtracting the
highlight frame and converts the operands before subtraction.  A widget whose
size is smaller than its frame therefore produces a zero drawing extent rather
than an unsigned wraparound that could request a huge X11 shadow region.

The shadow helper is called only when both extents are positive and both shadow
GCs exist.  Normal-sized buttons retain the existing geometry and shadow
ordering; the guard only makes the tiny or partially initialized state a safe
no-op.

The focused source regression is
`tests/automated/check-icon-button-redisplay-bounds.py`.
