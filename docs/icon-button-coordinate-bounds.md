---
nav_exclude: true
---

# IconButton coordinate bounds

IconButton clamps normal and insensitive-shadow label coordinates before
passing them to Motif's signed `Position` drawing arguments.  Oversized icon
layouts therefore cannot wrap text to an unrelated screen coordinate.

`tests/automated/check-icon-button-coordinate-bounds.py` protects this drawing
contract.
