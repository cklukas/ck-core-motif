---
nav_exclude: true
---

# Manager trait getter guards

Manager’s direction, unit-type, and access-colors trait callbacks now validate
the widget class and color-output structure before reading manager state.
Invalid inputs return safely (`XmDEFAULT_DIRECTION` or `XmPIXELS`, or no color
write); valid manager subclasses retain their configured values.

The focused source-contract check is
`python3 tests/automated/check-manager-trait-getter-guards.py`.
