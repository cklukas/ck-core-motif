---
nav_exclude: true
---
# Indicator drawing GC guard

`XmeDrawIndicator` dispatches checkmark and cross rendering through a shared
public helper. A partially initialized widget could reach it with a missing GC,
which would pass the null handle into Xlib.

The helper now returns before dispatch when either the drawable or indicator GC
is absent. Valid indicator rendering and the public API are unchanged. The
regression `tests/automated/check-drtog-indicator-gc-guard.py` covers the
boundary guard.
