---
nav_exclude: true
---

# Keep DnD read counts representable

`_XmReadDragBuffer` returns a `CARD16` byte count for ABI compatibility.  It
now caps each copy at `UINT16_MAX`, so the returned count always describes the
bytes actually copied instead of wrapping after a larger copy.
