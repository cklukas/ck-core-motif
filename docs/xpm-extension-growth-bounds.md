---
nav_exclude: true
---

# XPM extension growth bounds

Extension and extension-line arrays now check count-plus-one multiplication
before `XpmRealloc`, and extension counts stay representable by the public
`int` cleanup API. Initial extension allocation failure is also reported
immediately, preventing wrapped sizes or reliance on allocator-specific
`realloc(NULL, ...)` behavior.

Regression: `tests/automated/check-xpm-extension-growth-bounds.py`
