---
nav_exclude: true
---

# TabBox label bitmap graphics contexts

The horizontal and vertical TabBox label renderers now verify both lazily
created bitmap graphics contexts before clearing or drawing the label bitmap.
Under X resource pressure, a failed GC allocation therefore releases any
partial pair and returns safely instead of issuing `XFillRectangle` or text
drawing requests with a null GC. A later expose can retry allocation normally.

The focused source contract is checked by
`tests/automated/check-tabbox-label-gc.py`.
