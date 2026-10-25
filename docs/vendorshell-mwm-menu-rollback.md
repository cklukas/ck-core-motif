---
nav_exclude: true
---
# VendorShell MWM menu rollback

VendorShell now copies a changed `mwm_menu` resource before releasing the
previous owned copy.  When allocation fails during `SetValues`, the previous
menu remains installed; initialization safely leaves the menu unset if its
copy cannot be made.  This preserves MWM menu behavior without exposing a
dangling resource pointer.

The focused regression is
`tests/automated/check-vendorshell-mwm-menu-rollback.py`.
