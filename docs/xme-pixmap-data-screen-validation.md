---
nav_exclude: true
---
# Xme pixmap-data screen validation

`XmeGetPixmapData` obtains an application context through
`DisplayOfScreen(screen)`. It now rejects a null screen before that access and
returns `False`, while retaining its existing support for nullable result
pointers.

Valid screen queries and cache population are unchanged. The source-level
regression is `tests/automated/check-xme-pixmap-data-screen.py`.
