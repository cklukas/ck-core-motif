---
nav_exclude: true
---

# Screen font-unit input validation

`XmSetFontUnits` now ignores a missing display or unresolved screen, and
`_XmGetFontUnit` returns zero for a missing screen. Valid font-unit updates
and reads remain unchanged.

The focused regression check is
`tests/automated/check-screen-font-unit-input.py`.
