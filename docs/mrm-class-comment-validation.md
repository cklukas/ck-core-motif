---
nav_exclude: true
---
# Mrm class and comment validation failures

`UrmCWRSetClass` and `UrmCWRSetComment` now return the result of context
validation before dereferencing the widget record. Invalid or detached writer
contexts therefore fail consistently instead of continuing into record access.

The source-level regression is
`tests/automated/check-mrm-class-comment-validation.py`.
