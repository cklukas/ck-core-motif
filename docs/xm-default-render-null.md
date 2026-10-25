---
nav_exclude: true
---
# Default render-table null-safety

`XmeGetDefaultRenderTable` now returns `NULL` for a null widget before Xt
app-context, ancestor, or display traversal. Valid trait and fallback render
table lookup remains unchanged.
