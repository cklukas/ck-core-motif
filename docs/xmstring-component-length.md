# XmString component lengths

`XmeStringGetComponent` exposes rendition and tag lengths as `unsigned int`
while native string lengths are `size_t`.  The component path now checks the
conversion before narrowing and ends the context safely if a length cannot be
represented.  Copied component data is released on that failure path.
