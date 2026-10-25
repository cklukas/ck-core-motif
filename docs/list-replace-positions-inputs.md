---
nav_exclude: true
---
# Position-based list replacement input validation

`XmListReplacePositions` now rejects null widgets, position or item arrays,
non-positive counts, and null replacement strings before entering the
application context or indexing list state.

`tests/automated/check-list-replace-positions-inputs.py` protects this
contract.
