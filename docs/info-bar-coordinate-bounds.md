---
nav_exclude: true
---

# InfoBar coordinate bounds

InfoBar clamps prefix, message, and action-label positions before passing them
to Motif's signed `Position` drawing arguments.  Large widget dimensions or
font metrics therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-info-bar-coordinate-bounds.py` protects this drawing
contract.
