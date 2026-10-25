---
nav_exclude: true
---
# Render-table conversion array bounds

Render-table property conversion stores generated arguments and temporary
strings in fixed arrays.  Each insertion now checks the corresponding array
capacity and routes oversized input through the existing cleanup path, avoiding
out-of-bounds writes for unusually large property data.
