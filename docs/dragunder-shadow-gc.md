---
nav_exclude: true
---
# Drag-under shadow graphics contexts

`DrawShadow` now handles both graphics-context allocation failures. A missing
top context aborts before configuration; a missing bottom context releases the
top context and aborts. `FreeAnimationData` also checks optional contexts before
freeing them, so partial setup remains safe.

The focused source contract is checked by
`tests/automated/check-dragunder-shadow-gc.py`.
