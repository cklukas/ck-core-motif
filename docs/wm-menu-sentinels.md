---
nav_exclude: true
---
# WmMenu sentinel values

Mwm menu construction used pointer-valued `NULL` casts for KeySym, KeyCode and
CARD32 sentinel arguments.  Those casts trigger 64-bit pointer-to-integer
diagnostics even though the intended value is zero.

The calls now use explicit zero constants of the destination types.  Menu
construction behavior and public interfaces are unchanged.  The regression
check is `tests/automated/check-wm-menu-sentinels.py`.
