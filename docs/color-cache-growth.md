# Color cache growth failures

Motif keeps small process-wide caches for default colors and calculated color
sets. Initial allocation and growth now check count-to-byte conversion against
both host `size_t` and Xt's `Cardinal` limits, and pass explicit Xt-sized byte
counts to the allocator. A failed allocation leaves the existing cache and its
capacity unchanged instead of publishing a null pointer and continuing to
write through it.
