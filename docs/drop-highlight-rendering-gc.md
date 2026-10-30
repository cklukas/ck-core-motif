---
nav_exclude: true
---

# DropHighlight rendering graphics context

`XmDropHighlight` creates a short-lived graphics context for each expose
operation. The callback now returns when that allocation fails, before any
rectangle or label drawing request is sent to X. This avoids a `BadGC` while
preserving normal highlight rendering when resources are available.

The focused source contract is checked by
`tests/automated/check-drop-highlight-render-gc.py`.
