# Optimized XmString concatenation resize

When concatenation reuses an optimized string as a segment, its storage may
need to grow.  A failed `XtRealloc` now restores the string reference count,
releases both consumed inputs through the normal failure path, and returns
null before `memmove`, avoiding a null-pointer write or leaked ownership.
