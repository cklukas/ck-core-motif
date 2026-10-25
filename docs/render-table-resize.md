---
nav_exclude: true
---
# Render-table resize

When removal compacts a render table, the `XtRealloc` result is now checked
before replacing the table body. A failed resize or replacement handle leaves
the current table intact rather than publishing a null body.
