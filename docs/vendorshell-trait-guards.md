---
nav_exclude: true
---

# VendorShell trait guards

VendorShell’s direction and unit-type trait callbacks now handle null widgets
and incomplete shell-extension records before dereferencing extension state.
The established fallbacks remain unchanged: `XmRIGHT_TO_LEFT` for unavailable
direction state and `XmPIXELS` for unavailable unit state.

The focused source-contract check is
`python3 tests/automated/check-vendorshell-trait-guards.py`.
