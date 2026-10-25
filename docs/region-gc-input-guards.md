# XmRegion GC input guards

`_XmRegionSetGCRegion` now returns when its display or graphics-context handle
is absent, before rectangle collection or the Xlib clip operation. This keeps
optional region cleanup safe while preserving the existing behavior for valid
X11 handles and regions.

The focused regression is `tests/automated/check-region-gc-input-guards.py`.
