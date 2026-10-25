# Protocol registration allocation

Adding shell protocols allocates one per-protocol record before publishing the
new count. If a record allocation fails partway through a batch, the partially
created records are released and the manager keeps its original protocol
count, so later registration can retry without a leaked or half-initialized
entry.

The source-level regression is
`tests/automated/check-protocol-allocation.py`.


Batch growth now rejects `Cardinal` count and byte-size overflow and publishes
the expanded capacity only after `XtRealloc` succeeds.
