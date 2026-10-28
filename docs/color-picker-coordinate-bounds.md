---
nav_exclude: true
---

# ColorPicker coordinate bounds

ColorPicker clamps its hexadecimal value label before passing it to Motif's
signed `Position` drawing arguments.  Large picker heights or font metrics
therefore cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-color-picker-coordinate-bounds.py` protects this
drawing contract.
