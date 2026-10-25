# Mutable XmString entry-array resize

The mutable non-optimized concatenation path now checks its entry-array
`XtRealloc`.  On failure it restores a temporarily detached segment array,
releases the consumed inputs, and returns null before writing through the
failed allocation.
