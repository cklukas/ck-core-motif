---
nav_exclude: true
---

# Xft draw-cache creation failures

`_XmXftDrawCreate` now returns immediately when Xft cannot create a draw
object, rather than caching a null entry for the drawable. Subsequent calls can
therefore retry after the drawable or display state becomes usable.
