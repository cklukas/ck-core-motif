---
nav_exclude: true
---
# TabBox geometry allocation

`XmTabBox` keeps private arrays for actual tab rectangles, wanted tab
rectangles, and rendered image-cache entries. Growth now validates the count
and both host `size_t` and Xt `Cardinal` byte-size limits, retaining the
existing geometry arrays when
`XtRealloc` fails, and returns before the layout code can dereference a null
replacement.  Image-cache replacement allocates and checks the new cache
before publishing it, passing bounded byte counts explicitly as Xt `Cardinal`
values; an unavailable cache is represented as an empty cache.

The public tab ordering, geometry resources, and cache behavior on successful
allocation are unchanged.  Invalid or non-positive list counts now produce an
empty layout rather than indexing through an invalid allocation.
