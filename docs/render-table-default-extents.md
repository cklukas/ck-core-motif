---
nav_exclude: true
---
# Default render-table extents

`XmRenderTableGetDefaultFontExtents` accepts a null render table and initializes
any supplied height, ascent and descent outputs to zero. This preserves the
existing no-font result while avoiding a dereference during display-lock setup.
