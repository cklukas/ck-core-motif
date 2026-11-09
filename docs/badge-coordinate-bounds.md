---
nav_exclude: true
---

# Badge coordinate bounds

Badge clamps its label origin before passing it to Motif's signed `Position`
drawing arguments.  Large badge heights or font metrics therefore cannot wrap
the label to an unrelated screen coordinate.

The modern inner face is skipped when the outer allocation cannot contain it.
Its label clip rectangle also bounds X11 coordinates and extents before the
conversion to `XRectangle`, preventing large margins from wrapping the clip.

`tests/automated/check-badge-coordinate-bounds.py` protects this drawing
contract.
