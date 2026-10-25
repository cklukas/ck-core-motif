---
nav_exclude: true
---

# MWM transient-window list growth

Transient-tree enumeration grows a temporary window-ID list as it walks
siblings and children. Growth now checks the size arithmetic and propagates
`XtRealloc` failure through the recursive walk; the top-level builder frees its
partial list and returns `NULL` instead of dereferencing a failed allocation.
