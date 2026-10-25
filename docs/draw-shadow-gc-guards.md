# XmeDrawShadows graphics-context guards

`XmeDrawShadows` now returns when either shadow graphics context is missing.
Partially initialized or teardown-time widgets therefore avoid issuing Xlib
draw calls with an invalid GC, while normal shadow rendering is unchanged.

The source-level regression is `tests/automated/check-draw-shadow-gc-guards.py`.
