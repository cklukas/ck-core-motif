---
nav_exclude: true
---
# Focus callback context guard

`ChangeActiveIconboxIconText` now checks that the window-context lookup
returned a client record before evaluating `P_ICON_BOX`.  Valid focus handling
is unchanged.

`tests/automated/check-focus-context-guard.py` protects this contract.
