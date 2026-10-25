---
nav_exclude: true
---

# Screen pixmap/cache input validation

Scratch-pixmap release now safely ignores missing screens, `None` pixmaps, or
uninitialized cache tables. Cursor-cache lookup likewise returns `NULL` for a
missing screen. Valid cache allocation and release remain unchanged.

The focused regression check is
`tests/automated/check-screen-pixmap-cache-input.py`.
