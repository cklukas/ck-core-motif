# Color cache growth failures

Motif keeps small process-wide caches for default colors and calculated color
sets.  Their growth paths previously assigned `XtRealloc`'s result directly
to the cache pointer, then continued writing entries.  A failed allocation
could therefore discard the old cache and dereference a null pointer.

Growth now checks the count-to-byte conversion, calculates the new capacity
without signed overflow, and updates the cache pointer and capacity only
after the allocation succeeds.  Existing cache entries remain available when
the toolkit cannot grow the cache.
