---
nav_exclude: true
---

# TreeView coordinate bounds

TreeView keeps its row and indentation calculations in signed `int` values,
then clamps label coordinates before passing them to Motif's signed `Position`
arguments.  Large trees, indentation, or font metrics therefore cannot wrap a
label to an unrelated screen coordinate.

`tests/automated/check-tree-view-coordinate-bounds.py` protects this drawing
contract.
