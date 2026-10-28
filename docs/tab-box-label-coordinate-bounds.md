---
nav_exclude: true
---

# TabBox label coordinate bounds

TabBox clamps visible canvas tab labels, including the insensitive shadow
pass, before passing coordinates to Motif's signed `Position` drawing
arguments.  Oversized tab layouts therefore cannot wrap a label to an
unrelated screen coordinate.

`tests/automated/check-tab-box-label-coordinate-bounds.py` protects this
drawing contract.
