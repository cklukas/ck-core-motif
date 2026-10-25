---
nav_exclude: true
---
# Resource converter token allocation

`GetNextToken` now checks both its empty-token and normal-token allocations
before writing into the returned buffer. The normal token span is measured in
`size_t`, rejected if it cannot fit Xt's `Cardinal` range, and passed
explicitly to the allocator. Allocation failure returns `NULL` without
dereferencing invalid storage; callers retain their existing end-of-input
behavior.

The regression check is
`tests/automated/check-resconvert-token-allocation.py`.
