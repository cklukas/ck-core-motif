# Icon-box virtual-space expansion bounds

Virtual-space expansion now validates its icon-box state, dimensions, and
placement increments before division.  It also rejects products that cannot
fit in the placement-count integer, avoiding divide-by-zero and arithmetic
wraparound during resize and packing.

`tests/automated/check-expand-space-bounds.py` protects these resize bounds.
