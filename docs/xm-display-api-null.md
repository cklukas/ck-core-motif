---
nav_exclude: true
---
# Xm display and drag lookup null-safety

`XmGetDragContext` and `_XmGetDragContextFromHandle` now return `NULL` for a
null widget before Xt display lookup. `XmGetXmDisplay` returns `NULL` for a
null `Display`. Valid drag-context and display lookup behavior remains unchanged.
