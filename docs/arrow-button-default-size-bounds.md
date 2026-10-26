---
nav_exclude: true
---
# ArrowButton default-size bounds

When an `XmArrowButton` receives a zero requested width or height, Motif adds
its default 15-pixel extent.  That addition now saturates at the largest
representable X11 `Dimension` instead of wrapping a large existing value to a
small or zero size.  Ordinary zero-sized initialization remains unchanged.

The focused source regression is
`tests/automated/check-arrow-button-default-size-bounds.py`.
