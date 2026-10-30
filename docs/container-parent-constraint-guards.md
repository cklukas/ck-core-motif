---
nav_exclude: true
---
# Container parent-constraint guards

When an icon changes outline parent, `ConstraintSetValues` now checks that the
new parent still has a Container constraint record.  If it does not, the icon
is detached to the root level and its depth/visibility state is reset instead
of dereferencing stale parent state.  Existing parent relationships retain
their normal depth and visibility behavior.

The source contract is checked by
`tests/automated/check-container-parent-constraint-guards.py`.
