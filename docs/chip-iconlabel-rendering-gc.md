---
nav_exclude: true
---

# Chip and IconLabel rendering graphics contexts

`XmChip` and `XmIconLabel` now require a valid foreground graphics context
before their expose callbacks clear or draw the window. Graphics-context
allocation can fail under X resource pressure; returning without issuing
drawing requests avoids a server-side `BadGC` while leaving the widget safe to
recreate or destroy.

The focused source contract is checked by
`tests/automated/check-chip-iconlabel-render-gc.py`.
