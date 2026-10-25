---
nav_exclude: true
---
# Mrm resource-context resizing

`UrmResizeResourceContext` now keeps the old buffer owned by the context until
`XtRealloc` succeeds.  A failed resize returns `MrmFAILURE` without publishing
a null pointer or changing the recorded buffer size.

The source-level regression is
`tests/automated/check-mrm-context-realloc.py`.
