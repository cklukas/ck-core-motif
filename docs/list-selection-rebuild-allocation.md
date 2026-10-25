---
nav_exclude: true
---
# List selection rebuild allocation failures

The List widget now checks the allocations used while rebuilding selected item
and selected position arrays against both host `size_t` and Xt's `Cardinal`
limits. If either allocation fails or is unrepresentable, the corresponding
selection count is reset to zero and the widget keeps an empty, consistent
selection state instead of writing through a null pointer. Successful rebuilds
retain their existing ordering and selection behavior.
