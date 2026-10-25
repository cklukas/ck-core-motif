---
nav_exclude: true
---
# XmString context-copy allocation

`_XmStringContextCopy` now checks both the size calculation and allocation of
the active rendition-tag array.  Counts are promoted before multiplication and
the byte count is bounded by the `Cardinal` allocator interface.  If the size
is unrepresentable or the copy cannot allocate that optional array, the copied
context keeps its string and scalar state but exposes an empty active-rendition
set instead of overflowing or dereferencing a null pointer.
