# XmList internal-list growth

`XmList` keeps an internal `ElementPtr` array alongside its public item
array.  Insertion grows that array with `XtRealloc`.  The old code assigned
the result directly to the widget field, then moved existing entries.  If
allocation failed, the old pointer was lost and the move dereferenced a null
pointer.  The byte count was also formed without checking the `int` item
count against `size_t`.

The growth path now checks the count before multiplication and commits the
new pointer only after `XtRealloc` succeeds.  Existing Motif callers and
resource interfaces are unchanged; an allocation failure returns from the
insertion helper without corrupting the existing internal array.
