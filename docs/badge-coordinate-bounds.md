---
nav_exclude: true
---

# Badge coordinate bounds

Badge clamps its label origin before passing it to Motif's signed `Position`
drawing arguments.  Large badge heights or font metrics therefore cannot wrap
the label to an unrelated screen coordinate.

`tests/automated/check-badge-coordinate-bounds.py` protects this drawing
contract.
