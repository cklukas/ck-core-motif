---
nav_exclude: true
---
# Icon-box packing guards

`PackIconBox` now returns when its widgets, icon increments, or placement list
are unavailable.  This prevents invalid Xt access and divide-by-zero during
packing while preserving normal behavior for initialized icon boxes.

`tests/automated/check-pack-icon-box-guards.py` protects the contract.
