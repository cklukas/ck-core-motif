---
nav_exclude: true
---
# Default font-list cache allocation

The per-display default font-list cache now grows transactionally. Capacity is
updated only after the new cache block has been allocated and initialized; a
failed allocation returns `NULL` while preserving the existing cache and its
capacity.
