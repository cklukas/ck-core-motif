# MWM bitmap-cache initialization

The first bitmap lookup grows the per-screen cache from an empty state.  If
that allocation fails, the cache must remain in its empty state so a later
lookup can retry.  The initialization now assigns the pointer and capacity
only after the allocation succeeds; an allocation failure reports the error
and leaves `bitmapCacheSize == 0`.

This preserves normal cache behavior and avoids a permanent null-cache state
after one transient allocation failure.
