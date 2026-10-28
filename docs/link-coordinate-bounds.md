---
nav_exclude: true
---

# Link coordinate bounds

Link clamps its label coordinates before passing them to Motif's signed
`Position` drawing arguments.  Large widget heights or font metrics therefore
cannot wrap the label to an unrelated screen coordinate.

`tests/automated/check-link-coordinate-bounds.py` protects this drawing
contract.
