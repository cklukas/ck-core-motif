---
nav_exclude: true
---
# FileSB item filter allocation failure

When a File Selection Box restores its full-path filter field, it combines the
current directory and pattern in a temporary buffer. The component lengths now
remain `size_t` values and the combined size is checked against Xt's
`Cardinal` width before allocation. If sizing or allocation fails, the existing
filter text remains unchanged and the source strings are still released.

The regression check is
`tests/automated/check-filesb-item-filter-allocation.py`.
