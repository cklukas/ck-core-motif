# Scaled image cleanup ownership

The scaler creates an `XImage` with a null data pointer and then installs a
buffer allocated with `XtMalloc`.  Calling the default `XDestroyImage`
destructor directly would release that buffer through Xlib's allocator, which
does not preserve the allocator ownership contract.

Scaled-image teardown now frees the installed data with `XtFree`, clears the
pointer, and only then destroys the XImage structure.  The same helper is used
for allocation failure and normal cleanup paths.

The ownership contract is covered by
`tests/automated/check-image-scaling-destroy.py`.
