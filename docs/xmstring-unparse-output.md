---
nav_exclude: true
---
# XmString unparse output validation

`XmStringUnparse` now accepts only the documented charset, multibyte, wide
character, and no-text output types. Invalid values return `NULL` before any
component processing, so callers cannot receive an unterminated partial result.

The regression check is `tests/automated/check-xmstring-unparse-output.py`.
