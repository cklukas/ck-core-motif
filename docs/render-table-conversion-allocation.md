---
nav_exclude: true
---
# Render-table property conversion allocation

`XmRenderTableCvtToProp` now checks its initial conversion-buffer allocation
before copying the transfer vector.  Allocation failure unlocks the app
context and returns failure without dereferencing a null buffer; successful
conversion behavior is unchanged.

The regression check is
`tests/automated/check-render-table-conversion-allocation.py`.
