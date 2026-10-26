---
nav_exclude: true
---

# Drag export-target conversion results

`XmeDragSource` obtains the exported target list through the normal Motif
conversion callback.  Before passing it to `XmDragStart`, the result is now
required to be an `XA_ATOM` list in 32-bit format, with a non-null payload
when its count is non-zero.  Invalid conversion results are released and
reject the drag instead of being reinterpreted as a target array.

The regression check is
`tests/automated/check-drag-target-conversion-result.py`.
