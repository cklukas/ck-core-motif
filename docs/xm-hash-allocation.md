# Xm hash-table allocation

Motif's private hash tables are used by image, pixmap, GC, and widget caches.
Bucket-pool allocation now fails cleanly instead of dereferencing a null pool,
and its free-list setup no longer forms a one-past-the-end pointer.  Resizing
builds a replacement bucket array and publishes it only after allocation
succeeds, preserving the live table on failure.  Null table frees and insert
attempts are also harmless.

`tests/automated/check-xm-hash-allocation.py` protects these contracts.

`_XmAddHashEntry` now reports insertion failure.  The trait registry uses that
result to free its temporary key and return `False`, so an allocation failure
does not silently lose the registration or leak memory.
