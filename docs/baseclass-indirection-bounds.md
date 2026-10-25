---
nav_exclude: true
---
# BaseClass indirection-table bounds

`CreateIndirectionTable` now uses a `Cardinal` resource index and checks the
resource-count-to-byte conversion against both host `size_t` and Xt `Cardinal`
limits before allocating the pointer table. Invalid or unrepresentable sizes
return `NULL`; normal resource indirection is unchanged.
