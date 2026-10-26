# Xft draw-cache growth

The optional Xft draw cache grows when all existing slots are occupied.  The
growth arithmetic now checks the signed capacity and both `size_t` and Xt
`Cardinal` byte limits, uses a temporary realloc pointer, and destroys the new
Xft draw object if the cache cannot be enlarged.  Existing cache entries remain
valid on failure.
