---
nav_exclude: true
---
# Rendition creation

`_XmRenditionCreate` now checks both its body and handle allocations. If
either allocation fails, it releases any partial body and returns `NULL`
without entering resource lookup or validation with an invalid handle.
