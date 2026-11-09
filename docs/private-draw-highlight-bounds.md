---
nav_exclude: true
---
# Private highlight segment bounds

The historical `_XmDrawHighlight` renderer is still used by legacy List and
IconG paths.  It creates X11 `XSegment` records, whose endpoints are signed
16-bit values.  Thickness is now limited to the supplied rectangle and every
derived endpoint is saturated before storage.  Normal highlight rendering is
unchanged; extreme geometry can no longer wrap a segment across the drawable.
