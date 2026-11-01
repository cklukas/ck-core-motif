---
nav_exclude: true
---
# ColorSelector argument merging

ColorSelector builds child widgets by merging caller arguments with internal
resource defaults.  The shared merge helper first rejects a `Cardinal` count
that would wrap, then checks the allocated merged list before use.  If Xt
cannot allocate a merged list, the affected helper returns cleanly instead of
passing or dereferencing a null list; successful child construction and
argument ownership remain unchanged.

The regression check is `tests/automated/check-colors-argument-merge.py`.
