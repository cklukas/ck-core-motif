# Container dynamic tab-pool bounds

Dynamic detail tabs are built from a per-call temporary pool. Its checked
`size_t` byte count is now converted explicitly to Xt's `Cardinal` allocator
size after the existing overflow guards, keeping the allocation contract
visible on both 32-bit and 64-bit builds.

The focused source contract is checked by
`tests/automated/check-container-tab-pool-allocation.py`.
