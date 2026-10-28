# Queue cleanup without a temporary stack

`_XmQueueFree` now has an allocation-free fallback for cleanup.  If its
temporary stack cannot be created, it walks the active and free block lists
directly and releases each allocated block, instead of leaking the queue's
storage during memory pressure.

The source-contract regression is
`tests/automated/check-xmlist-queue-free-fallback.py`.
