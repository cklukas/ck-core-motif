---
nav_exclude: true
---
# XmString text concatenation null values

`_XmStringGetTextConcat` now rejects malformed non-empty text components whose
value pointer is `NULL`. It releases the partial output and string context
before returning `NULL`, avoiding a crash in `memcpy` while preserving normal
empty-component behavior.

The regression check is
`tests/automated/check-xmstring-text-concat-null-value.py`.
