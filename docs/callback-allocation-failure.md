---
nav_exclude: true
---

# Callback-list allocation failure

Internal Motif callback lists can be copied or resized while callbacks are
being called. Allocation failure is now handled transactionally: adding a
callback leaves the active list intact, and removing a callback preserves the
current list when a replacement cannot be allocated. Deferred-free state is
also cleared when a copy cannot be made, preventing a dangling callback-list
pointer after callback dispatch completes.

The focused regression check is
`tests/automated/check-callback-allocation-failure.py`.
