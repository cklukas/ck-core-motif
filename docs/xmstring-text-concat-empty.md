---
nav_exclude: true
---
# XmString empty text concatenation

`_XmStringGetTextConcat` now skips `memcpy` for zero-length text components.
Empty components with null values remain valid, without relying on undefined
zero-size null-pointer copy behavior.

The regression check is
`tests/automated/check-xmstring-text-concat-empty.py`.
