---
nav_exclude: true
---

# DropHighlight coordinate bounds

DropHighlight clamps its centered label coordinates before passing them to
Motif's signed `Position` drawing arguments.  Large highlight dimensions or
font metrics therefore cannot wrap the label to an unrelated screen
coordinate.

`tests/automated/check-drop-highlight-coordinate-bounds.py` protects this
drawing contract.
