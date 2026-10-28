---
nav_exclude: true
---

# TimePicker coordinate bounds

TimePicker clamps its formatted time label before passing it to Motif's signed
`Position` drawing arguments.  Large picker heights or font metrics therefore
cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-time-picker-coordinate-bounds.py` protects this drawing
contract.
