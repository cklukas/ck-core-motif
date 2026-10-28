---
nav_exclude: true
---

# IconG coordinate bounds

IconG clamps label and detail text coordinates, including right-to-left detail
placement and the insensitive shadow pass, before passing them to Motif's
signed `Position` drawing arguments.  Large gadget layouts therefore cannot
wrap text to an unrelated screen coordinate.  Its XRectangle clip origins use
the same bounded conversion instead of an explicit wrapping `short` cast.
The large-icon mask clip origin follows the same rule.

`tests/automated/check-icon-g-coordinate-bounds.py` protects this drawing
contract.
