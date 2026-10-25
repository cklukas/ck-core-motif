---
nav_exclude: true
---

# Trait replacement ownership

`XmeTraitSet` now replaces an existing `(object, name)` binding instead of
stacking duplicate hash entries. The superseded key is released after the new
binding succeeds; if allocation fails, the old binding is restored when
possible. Removing a trait still releases its key as before.

The focused regression check is
`tests/automated/check-trait-replacement.py`.
