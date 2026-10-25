# Color cache growth failures

Motif keeps small process-wide caches for default colors and calculated color
sets. Their growth paths now check count-to-byte conversion and allocate into
temporary pointers. A failed allocation leaves the existing cache and its
capacity unchanged instead of publishing a null pointer and continuing to
write through it.
