---
nav_exclude: true
---
# XPM zero-height image cleanup

`CreateXImage` rejects zero-height images because there is no usable image
buffer to allocate. The rejection now destroys the `XImage` created by
`XCreateImage` and clears the caller's output pointer first. This keeps the
existing `XpmNoMemory` result and valid-image behavior while preventing a
resource leak for empty or malformed XPM input.

The focused regression is `tests/automated/check-xpm-zero-height-cleanup.py`.
