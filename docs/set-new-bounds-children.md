---
nav_exclude: true
---
# Icon-box bounds child guards

`SetNewBounds` now handles a missing composite child list and skips null child
entries while recalculating icon-box bounds.  Valid child geometry continues to
drive the same bounds.

`tests/automated/check-set-new-bounds-children.py` protects the contract.
