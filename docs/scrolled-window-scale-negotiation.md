---
nav_exclude: true
---
# ScrolledWindow scale negotiation

When a ScrolledWindow uses a toolkit-owned default size, a live display-scale
change must request the new preferred size through Xt. Calling `XtResizeWidget`
directly can violate a parent Form or other fixed allocation: the child shrinks
even though its parent still owns the previous allocation. `ScaleChanged` now
uses `XtMakeResizeRequest`, accepts an `Almost` reply through the normal second
request, and records the actual accepted core dimensions for subsequent scale
changes. Explicit application dimensions remain untouched.

The source contract is checked by
`tests/automated/check-scrolled-scale-negotiation.py`; the installed live-scale
consumer is `tests/automated/run-display-scale-scrolled.py`.
