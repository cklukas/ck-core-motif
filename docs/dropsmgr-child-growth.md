# Drop-site child-list growth

Composite drop-site metadata stores child records in a growable list.  The
growth path now validates the 16-bit child-count addition and byte size before
resizing, uses a temporary pointer, and publishes the new capacity only after
`XtRealloc` succeeds.  A failed resize leaves the existing drop-site tree
unchanged.
