---
nav_exclude: true
---
# XPM zero-height image cleanup

`CreateXImage` rejects zero-height images because there is no usable image
buffer to allocate. It also rejects dimensions whose row-stride multiplication
would overflow. Both rejection paths now destroy the `XImage` created by
`XCreateImage` and clear the caller's output pointer first. This keeps the
existing `XpmNoMemory` result and valid-image behavior while preventing a
resource leak or dangling output pointer for malformed XPM input.

The focused regression is `tests/automated/check-xpm-zero-height-cleanup.py`.
