# Default drag target-table allocation handling

`CreateDefaultTargetsTable` builds the initial process-local target cache.
It now checks the table, entry-array, and default `XA_STRING` target-list
allocations, releasing earlier allocations and returning `NULL` when memory
is unavailable.  Callers can retain their existing null-table handling
instead of entering the cache with partially initialized pointers.
