---
nav_exclude: true
---
# List hit-test edge bounds

`WhichItem` rejects pointer positions above the first list row by comparing the
event Y coordinate with `BaseY - HighlightThickness`.  Those fields have
different signedness and widths; subtracting them before converting to
`Position` could wrap during a tiny or partially initialized allocation.

The comparison now widens both operands before subtracting.  Normal row
selection is unchanged, while the top boundary remains deterministic during
transient geometry.  The contract is protected by
`tests/automated/check-list-hit-test-edge-bounds.py`.
