---
nav_exclude: true
---

# Mrm deferred bitmap arguments

`Urm__CW_CreateArglist` uses a legacy ten-entry staging array while deferring
icon-image and bitmap-file resource conversion until foreground and background
values are known.  Every bitmap-file path now applies the same capacity guard
as icon images.  Additional deferred bitmap resources report the established
Mrm failure and are skipped instead of writing past that array.

The focused regression is
`tests/automated/check-mrm-deferred-bitmap-arguments.py`.
