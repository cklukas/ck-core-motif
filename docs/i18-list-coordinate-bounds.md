---
nav_exclude: true
---

# I18List coordinate bounds

I18List clamps column-header and row text coordinates before passing them to
Motif's signed `Position` drawing arguments.  Large list geometry or scrolling
offsets therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-i18-list-coordinate-bounds.py` protects this drawing
contract.
