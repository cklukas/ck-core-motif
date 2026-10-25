---
nav_exclude: true
---
# MWM workspace-restore allocation

Session and proxy-client restoration can return workspace-name metadata to
the window manager.  The copied string is now assigned only after
`XtNewString` succeeds, so an allocation failure preserves the caller’s
existing pointer and still allows the rest of client-state restoration to
complete.

`tests/automated/check-mwm-workspace-restore-allocation.py` protects this
contract.
