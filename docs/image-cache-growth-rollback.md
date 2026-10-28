# Image-cache growth rollback

`CacheColorPixel` now computes the proposed cache capacity separately and
publishes it only after `XtRealloc` succeeds.  If growth fails, the existing
capacity remains authoritative, so a later insertion cannot mistake an
unallocated range for usable cache storage.

The source-contract regression is
`tests/automated/check-image-cache-growth-rollback.py`.
