---
nav_exclude: true
---
# MWM system-menu placement input validation

`GetSystemMenuPosition` is a legacy `void` helper.  It now returns without
writing coordinates when its client record or either output pointer is null.
Normal minimized, icon-box, and framed-client placement calculations remain
unchanged for valid calls.

The source-contract regression is
`tests/automated/check-mwm-system-menu-position-inputs.py`.
