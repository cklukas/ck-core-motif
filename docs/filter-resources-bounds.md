---
nav_exclude: true
---

# Legacy resource filtering bounds

`_XmFilterResources` now counts matching resources before allocation and fills
the result in a second pass. This removes the former fixed 256-entry temporary
index limit, checks the result-size arithmetic for both `size_t` and Xt's
`Cardinal`, and reports allocation failure
with an empty result instead of writing through `NULL`.
Null resource, class, or output pointers now return an empty result before
the filter is inspected.

The focused regression check is
`tests/automated/check-filter-resources-bounds.py`.
