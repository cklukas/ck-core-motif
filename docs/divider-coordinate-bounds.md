---
nav_exclude: true
---

# Divider coordinate bounds

Divider clamps its centered horizontal label before passing it to Motif's
signed `Position` drawing arguments.  Large divider dimensions or font metrics
therefore cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-divider-coordinate-bounds.py` protects this drawing
contract.
