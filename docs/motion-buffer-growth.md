# Bound motion-buffer growth

Motion-event batching now checks both size arithmetic and Xt's `Cardinal`
allocation limit when promoting and expanding its buffer.  Initial heap
promotion and later `XtRealloc` failure return without writing through an
invalid pointer; count overflow is rejected before calculating the new size.

The focused source regression is
`tests/automated/check-motion-buffer-growth.py`.
