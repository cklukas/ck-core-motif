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

## Geometry negotiation

Proportional fill uses the original requested preferred dimensions while a
geometry compromise is outstanding. When a child accepts that offer, ButtonBox
uses the same allocation rather than treating the accepted share as a new
preferred size and shrinking it again. Subsequent layout or competing geometry
requests invalidate the offer. Query-only acceptance leaves geometry and the
offer unchanged. This requires the pending ABI-9 constraint record.

`run-buttonbox-negotiation.py` exercises both orientations, equal and unequal
sizing, every fill mode, and zero/small/large requests. Every offered compromise
must be accepted on the next request, including a preceding query-only check.
The current native matrix passes 123 compromise acceptances in each build.
