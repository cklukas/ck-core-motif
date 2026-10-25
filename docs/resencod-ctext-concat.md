Compound-text concatenation failure handling
=============================================

`ResEncod.c` compound-text concatenation now checks the combined byte count
before growth and keeps the original buffer when `XtRealloc` cannot extend it.
It no longer overwrites that buffer with `NULL` before copying, preventing a
null dereference and preserving already-encoded output under memory pressure.
