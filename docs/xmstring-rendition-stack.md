---
nav_exclude: true
---
# XmString rendition-stack extents

Segment extent calculation maintains a temporary stack of rendition tags.
The old begin/end paths assigned `XtRealloc` results directly before copying
or compacting the stack.

The paths now check the combined tag count and commit growth only after
success.  A failed shrink keeps the existing larger buffer, which remains
valid for the reduced tag count.
