---
nav_exclude: true
---

# MenuShell direction trait guard

MenuShell’s layout-direction trait now validates the widget class before
reading shell state. Invalid inputs return `XmDEFAULT_DIRECTION`; valid
MenuShell subclasses retain their configured direction.

The focused source-contract check is
`python3 tests/automated/check-menushell-direction-guard.py`.
