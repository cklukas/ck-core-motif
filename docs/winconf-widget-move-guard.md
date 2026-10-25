---
nav_exclude: true
---
# Window-configuration widget move guard

The icon-box move path now validates placement storage, the current index, and
the target widget before calling `XtMoveWidget`.  Valid icon moves retain their
existing behavior.

`tests/automated/check-winconf-widget-move-guard.py` protects the contract.
