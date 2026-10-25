---
nav_exclude: true
---

# Toolbar action-label allocation guards

`XmToolbar` now handles conversion and allocation failures while composing an
action label with its shortcut. Length arithmetic is checked before allocation,
and the existing label formatting is unchanged for valid inputs.

The focused regression check is
`tests/automated/check-toolbar-action-label-guards.py`.
