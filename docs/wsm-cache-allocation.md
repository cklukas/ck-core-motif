# WSM display and screen cache allocation

WSM keeps display and screen metadata in lazily allocated linked lists.  The
old code immediately stored and dereferenced newly allocated nodes, so a
failed allocation could crash before callers had a chance to handle the
missing configuration.  The cache now checks every new node, leaves the list
unchanged on failure, emits an Xt warning, and returns `NULL` through the
existing lookup APIs.

Configuration cleanup also returns immediately when the display record is
unavailable, so teardown after a failed cache allocation cannot dereference a
null display entry.

`tests/automated/check-wsm-cache-allocation.py` protects these failure paths.
