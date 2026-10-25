---
nav_exclude: true
---
# InfoBar redisplay guards

`XmInfoBar` now treats a missing drawing GC or an unavailable X11 font as a
non-rendering state and returns safely from `Redisplay`. X11 can report either
allocation failure, and neither result is safe to dereference. The normal
rendering path and callback behavior are unchanged.

The font metrics are obtained from the GC with `XGetGCValues(..., GCFont,
...)` before calling `XQueryFont`. A GC context ID is not a font ID; using
`XGContextFromGC` here caused `BadFont` on servers with independent X11
resource namespaces.

The focused source contract is checked by
`tests/automated/check-info-bar-redisplay-guard.py`.
