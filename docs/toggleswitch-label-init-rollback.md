---
nav_exclude: true
---
# ToggleSwitch label initialization rollback

ToggleSwitch now checks the private copy of its initial label.  If
`XmStringCopy` fails, it installs an empty compound string instead of passing a
NULL label to geometry measurement and drawing.  This keeps construction safe
and preserves a usable control under allocation pressure.

The focused regression is
`tests/automated/check-toggleswitch-label-init-rollback.py`.
