---
nav_exclude: true
---
# Label visual-string rollback

`XmLabel` now copies replacement label and accelerator strings before freeing
the current widget-owned values.  If an `XmString` allocation fails, the
current text or accelerator remains installed and the requested external
pointer is discarded without leaving a partially updated resource.

The focused regression is
`tests/automated/check-label-visual-string-rollback.py`.
