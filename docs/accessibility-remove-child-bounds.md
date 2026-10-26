---
nav_exclude: true
---
# Accessibility child-removal bounds

`XmAccessibilityRemove` now validates the parent child array, the recorded
child index and identity, and the count/capacity relationship before
decrementing the count or shifting survivors. Removing the final child
performs no shift.
Malformed or stale semantic-tree metadata therefore leaves the tree intact
instead of underflowing the move length and passing an oversized range to
`memmove`; a stale in-range index cannot shift a different sibling.

The focused source-contract check is
`tests/automated/check-accessible-remove-child-bounds.py`.
