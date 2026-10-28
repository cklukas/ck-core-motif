---
nav_exclude: true
---

# Avatar coordinate bounds

Avatar clamps the centered initials label before passing it to Motif's signed
`Position` drawing arguments.  Large avatar dimensions or font metrics
therefore cannot wrap initials to an unrelated screen coordinate.

`tests/automated/check-avatar-coordinate-bounds.py` protects this drawing
contract.
