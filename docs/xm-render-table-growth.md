---
nav_exclude: true
---

# Render-table growth bounds

`XmRenderTableCvtFromProp` grows its rendition array in fixed increments. The
growth check now rejects sizes that overflow either `size_t` or Xt's
`Cardinal` allocation width before calling `XtRealloc`, preserving the
property-conversion cleanup path on oversized input.

The optional Xft color cache applies the same checks before growing its
per-process color array, so a large cache cannot wrap the `Cardinal` byte
count.

The focused regression check is
`tests/automated/check-xm-render-table-growth.py`.
