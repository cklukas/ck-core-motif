---
nav_exclude: true
---
# ButtonBox size bounds

ButtonBox accumulated child sizes, borders, and margins directly in
`Dimension`.  Large child counts or dimensions could wrap totals and make the
parent request an unrelated small geometry.  Child totals, preferred sizes,
and layout requests now compute in `size_t` and saturate at the maximum
representable `Dimension`.

`tests/automated/check-buttonbox-size-bounds.py` keeps the bounded paths in
place.
