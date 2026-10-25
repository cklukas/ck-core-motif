---
nav_exclude: true
---
# Arrow drawing GC guards

`XmeDrawArrow` treats the center GC as optional for flat arrows, but shadowed
arrows require both top and bottom GCs. The old implementation only checked
the drawable and passed missing shadow GCs to `XFillRectangles`, which can
produce an Xlib error during partial widget initialization or teardown.

The helper now returns before allocating or drawing when a shadowed arrow lacks
either required GC. Flat arrows retain their existing optional-center behavior;
the public API and normal geometry are unchanged.
