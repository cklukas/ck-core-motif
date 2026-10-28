---
nav_exclude: true
---

# IconButton coordinate bounds

IconButton clamps normal and insensitive-shadow label coordinates before
passing them to Motif's signed `Position` drawing arguments.  Oversized icon
layouts therefore cannot wrap text to an unrelated screen coordinate.
Pixmap and cached text origins are bounded during layout as well as at draw
time, so later clipping and hit-testing see the same safe coordinates.
Text origins derived from pixmap size, padding, and fallback spacing are also
clamped before being stored in the widget record.

`tests/automated/check-icon-button-coordinate-bounds.py` protects this drawing
contract.
