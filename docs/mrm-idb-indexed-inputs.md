# Mrm indexed-resource input checks

`UrmIdbGetIndexedResource` requires an index string and a resource context for
the returned data block. It now rejects either null input before invoking the
index and data-layer helpers, preventing null dereferences while preserving the
existing behavior for valid lookups.
