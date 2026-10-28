---
nav_exclude: true
---

# List base-position bounds

List now clamps its margin, highlight, and shadow sums before storing BaseX
and BaseY in Motif's signed `Position` fields.  Initialization, redraw, and
scrollbar-visibility paths use the same bounded conversion, preventing large
application margins from wrapping content geometry.

`tests/automated/check-list-base-position-bounds.py` protects this contract.
