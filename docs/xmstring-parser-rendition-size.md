# XmString parser rendition-array sizing

The non-optimized compound-string parser now allocates rendition-begin and
rendition-end arrays using element count multiplied by `sizeof(XmStringTag)`.
The previous count-only allocation under-allocated on normal 64-bit builds;
size and allocation failures now return `NULL` before the array is written.
