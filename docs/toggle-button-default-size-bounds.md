---
nav_exclude: true
---
# ToggleButton default-size bounds

`XmToggleButton` derives its default geometry from label extents, margins,
indicator size, and highlight/shadow thickness.  Those sums now use widened
arithmetic and clamp to the X11 `Dimension` range before assignment.  Normal
toggle sizing is unchanged, while large resource values no longer wrap into
unrelated small dimensions.

The focused source regression is
`tests/automated/check-toggle-button-default-size-bounds.py`.
