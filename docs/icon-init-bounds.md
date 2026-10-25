---
nav_exclude: true
---
# Icon-box initialization bounds

Icon-box initialization now handles zero or negative icon dimensions and
screen/layout products that cannot fit in the placement-count integer.  It
falls back to one placement slot before allocating the placement array and
rejects any unrepresentable allocation size.

`tests/automated/check-icon-init-bounds.py` protects these startup bounds.
