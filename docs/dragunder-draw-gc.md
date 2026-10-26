---
nav_exclude: true
---
# Drag-under shared drawing context

`CreateAnimationSaveData` now treats failure to create its shared drawing GC as
initialization failure. It releases the save record and returns `NULL`, which
lets the drag callback abort before any animation path can issue requests with
an invalid context.

The focused source contract is checked by
`tests/automated/check-dragunder-draw-gc.py`.
