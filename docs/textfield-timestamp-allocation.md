---
nav_exclude: true
---
# TextField timestamp allocation

The TextField timestamp selection target now checks its `Time` allocation
before writing the timestamp.  Allocation failure returns `False` instead of
dereferencing a null pointer.

Regression coverage: `tests/automated/check-textfield-timestamp-allocation.py`.
