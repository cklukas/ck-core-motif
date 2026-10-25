# Keep icon cache pointer conversions explicit

The icon-directory cache stores opaque pointers to a union so invalid and
valid cache entries share one table.  `CheckDirCache` consumes valid entries
through the more specific `DtValidCachedDir` pointer.  The code now makes the
two representation-preserving conversions explicit when inserting and
retrieving entries.

This documents the intended layout and avoids incompatible-pointer-type
errors from strict compilers without changing the cache ABI or allocation
ownership.
