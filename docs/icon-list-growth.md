---
nav_exclude: true
---
# Icon-list growth bounds

Icon-box virtual-space growth now rejects null state, non-positive increments,
signed integer overflow, and sizes that cannot fit in an Xt allocation before
performing pointer arithmetic or allocation.

`tests/automated/check-icon-list-growth.py` protects these layout bounds.
