# XmString tag-cache bounds and growth

The internal tag cache uses a zero-based index and `_cache_count` as its
element count.  The lookup helper previously accepted `_cache_count` itself,
which is one past the last element.  Cache initialization and growth also
used unchecked allocation results.

Lookups now reject negative and out-of-range indices.  Initial and expanded
cache storage is committed only after allocation succeeds, and the public
internal cache helper returns `NULL` when no tag can be stored.  Existing
valid tag indices and string representations are unchanged.
