---
nav_exclude: true
---
# Drag-under highlight graphics context

`DrawHighlight` now checks the graphics context returned by `XCreateGC` before
applying a clip region or drawing. If the server cannot create the context,
highlight rendering is skipped safely instead of issuing requests through a
null GC.

The focused source contract is checked by
`tests/automated/check-dragunder-highlight-gc.py`.
