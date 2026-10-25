---
nav_exclude: true
---
# Default render-table extents

`XmRenderTableGetDefaultFontExtents` now handles a null render table without
dereferencing it. Any supplied height, ascent, and descent outputs are set to
zero, matching the function's existing no-font result behavior.
