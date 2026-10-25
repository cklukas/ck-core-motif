---
nav_exclude: true
---
# WmGraphics size-limit header

`clients/mwm/WmGraphics.c` bounds rectangle-list allocation with `INT_MAX` and
`UINT_MAX`.  The source now includes `<limits.h>` directly so Mwm compiles
without relying on an incidental header to provide those constants.

The allocation behavior and public interfaces are unchanged.  The automated
check is `tests/automated/check-wm-graphics-size-limit-header.py`.
