---
nav_exclude: true
---
# Mrm deferred pixmap bounds

`Urm__CW_CreateArglist` now checks the ten-entry deferred pixmap array before
saving icon-image or X bitmap arguments.  Excess deferred entries are rejected
without writing past the stack array; existing argument lists within the
historic capacity retain their behavior.

The source-level regression is
`tests/automated/check-mrm-pixmap-defer-bounds.py`.
