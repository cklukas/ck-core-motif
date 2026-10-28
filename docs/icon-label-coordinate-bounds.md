---
nav_exclude: true
---

# IconLabel coordinate bounds

IconLabel clamps its label origin in both left-to-right and right-to-left
layout modes before passing it to Motif's signed `Position` drawing arguments.
Large icons, spacing, or font metrics therefore cannot wrap text to an
unrelated screen coordinate.

`tests/automated/check-icon-label-coordinate-bounds.py` protects this drawing
contract.
