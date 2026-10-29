---
nav_exclude: true
---
# Container placement position bounds

`PlaceCwid` clamps children to Container margins and, for right-to-left
layouts, to the right content edge.  The old code cast unsigned dimensions to
`Position` before adding or subtracting margins.  `HideCwid` had the same issue
when moving a child to a negative off-screen coordinate.

Both paths now widen dimensions and margins first, then use the Container
saturating position helper.  Ordinary placement is unchanged; undersized or
oversized transient geometry no longer wraps to an unrelated screen position.
The contract is protected by
`tests/automated/check-container-place-position-bounds.py`.
