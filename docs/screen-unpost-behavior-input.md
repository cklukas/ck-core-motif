---
nav_exclude: true
---

# Screen unpost-behavior input validation

`_XmGetUnpostBehavior` now returns Motif’s default
`XmUNPOST_AND_REPLAY` behavior when its widget or screen is missing. Valid
screen-specific behavior remains unchanged.

The focused regression check is
`tests/automated/check-screen-unpost-behavior-input.py`.
