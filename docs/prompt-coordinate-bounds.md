---
nav_exclude: true
---

# Prompt coordinate bounds

Prompt clamps its text origin before passing it to Motif's signed `Position`
drawing arguments.  Large prompt heights or font metrics therefore cannot
wrap the text to an unrelated screen coordinate.

`tests/automated/check-prompt-coordinate-bounds.py` protects this drawing
contract.
