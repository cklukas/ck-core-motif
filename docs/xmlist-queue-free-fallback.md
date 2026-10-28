# Queue cleanup without a temporary allocation

`_XmQueueFree` now walks the active and free block lists directly. Teardown no
longer depends on a temporary stack (or on that stack growing successfully),
so every allocated block is released during memory pressure.

The source-contract regression is
`tests/automated/check-xmlist-queue-free-fallback.py`.
