---
nav_exclude: true
---
# Container SetValues child-constraint guards

When a Container changes from outline/detail to spatial layout, its
`ConstraintSetValues` method now skips children whose private Container
constraint record is unavailable.  This protects the transition-time
visibility update from dereferencing `entry_parent` on malformed or partially
initialized children.  Valid child transitions retain their existing behavior.

The source contract is checked by
`tests/automated/check-container-setvalues-constraint-guards.py`.
