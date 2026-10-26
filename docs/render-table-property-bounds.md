---
nav_exclude: true
---
# Render-table property bounds

`XmRenderTableCvtFromProp` keeps parser offsets in `int` for compatibility
with the existing token reader.  The converter now rejects property lengths
that cannot be represented by those offsets before parsing begins.  Normal
render-table resources are unchanged; oversized malformed properties fail
closed instead of truncating a pointer difference.

`tests/automated/check-render-table-property-bounds.py` is the focused source
regression for the boundary.
