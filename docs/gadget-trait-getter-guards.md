---
nav_exclude: true
---

# Gadget unit and direction trait guards

Gadget’s unit-type and layout-direction trait getters now validate the widget
class before reading instance state. Invalid inputs return the established
safe defaults (`XmPIXELS` and `XmDEFAULT_DIRECTION`); valid gadget subclasses
retain their configured values.

The focused source-contract check is
`python3 tests/automated/check-gadget-trait-getter-guards.py`.
