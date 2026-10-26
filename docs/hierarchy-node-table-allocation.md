# Hierarchy node-table allocation

`BuildNodeTable` rebuilds the hierarchy's constraint table from the number of
managed children.  The byte-size check now covers both `size_t` and Xt's
`Cardinal` allocation range before multiplication.  The existing table remains
published when `XtRealloc` fails, so the hierarchy does not dereference a
truncated or null allocation.
