---
nav_exclude: true
---
# I18List visible-size bounds

`XmI18List` previously multiplied row height by the visible-row count in an
`int` and then narrowed the result to an X11 `Dimension`.  Large or malformed
row metrics could therefore wrap and produce a too-small widget.  The layout
now computes content and title extents in `uint64_t`, treats non-positive row
counts as empty, and clamps the final height to the representable
`Dimension` range.

The focused source regression is
`tests/automated/check-i18list-visible-size-bounds.py`.
