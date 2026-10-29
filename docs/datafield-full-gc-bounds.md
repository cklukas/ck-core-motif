# DataField full-GC clipping bounds

The DataField full-widget clip rectangle now uses a widened inset and an
explicit empty-content check. Previously, subtracting twice the shadow and
highlight thickness from a tiny unsigned widget width or height could wrap to
an enormous `XRectangle`, causing drawing to escape the widget during resize.

The implementation mirrors the TextField clipping contract: the origin is
saturated to the X11 signed coordinate range, while width and height become
zero when the inset consumes the available allocation.

The source contract is checked by:

```text
python3 tests/automated/check-datafield-full-gc-bounds.py
```
