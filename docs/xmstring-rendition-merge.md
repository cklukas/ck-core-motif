# XmString rendition merge allocation failures

Concatenating XmStrings can merge rendition begin and end tag arrays.  The
merge helpers previously stored `XtRealloc`'s return value immediately and
then copied the second array into it.  Allocation failure could therefore
turn a valid rendition into a null-pointer write.

The helpers now compute the combined count in `int`, check its conversion to
an allocation byte count including Xt's `Cardinal` limit, and assign the new
pointer only after `XtRealloc` succeeds.  A failed merge leaves the original
tag array and count unchanged.
