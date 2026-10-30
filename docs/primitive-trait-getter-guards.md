---
nav_exclude: true
---

# Primitive trait getter guards

Primitive’s direction, access-colors, and unit-type trait callbacks now
validate their widget class and color-output structure before reading instance
state. Invalid inputs return safely (`XmDEFAULT_DIRECTION` or `XmPIXELS`, or
no color write); valid primitive subclasses retain their configured values.

The focused source-contract check is
`python3 tests/automated/check-primitive-trait-getter-guards.py`.
