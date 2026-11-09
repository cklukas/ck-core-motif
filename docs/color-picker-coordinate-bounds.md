---
nav_exclude: true
---

# ColorPicker coordinate bounds

ColorPicker clamps its hexadecimal value label before passing it to Motif's
signed `Position` drawing arguments.  It also bounds every pencil-tip
`XPoint` before passing it to X11's signed 16-bit drawing protocol.  Large
picker heights, font metrics, or explicit swatch allocations therefore cannot
wrap a label or a pencil triangle to an unrelated screen coordinate.

`tests/automated/check-color-picker-coordinate-bounds.py` protects this
drawing contract.
