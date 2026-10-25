# XmeDrawHighlight graphics-context guard

`XmeDrawHighlight` now returns when its graphics context is missing.  Widgets
being initialized or torn down therefore avoid invalid Xlib fill calls while
retaining the existing highlight behavior for valid resources.

The source-level regression is `tests/automated/check-draw-highlight-gc-guards.py`.
