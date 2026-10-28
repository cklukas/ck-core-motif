---
nav_exclude: true
---

# XPM extension-line initial allocation

The parser now checks the initial extension-line pointer-array allocation and
reports `XpmNoMemory` immediately, rather than relying on
`XpmRealloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-extension-lines-initial-allocation.py`
