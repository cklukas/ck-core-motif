---
nav_exclude: true
---

# Legacy shadow-buffer allocation

`_XmDrawShadow` and `XmDrawEtchedShadow` now grow their cached rectangle
buffers transactionally. Initial allocation failure leaves drawing state
unchanged and returns; reallocation failure preserves the existing buffer for
later calls. Successful drawing and rectangle geometry are unchanged.

The focused regression check is
`tests/automated/check-draw-shadow-allocation.py`.
