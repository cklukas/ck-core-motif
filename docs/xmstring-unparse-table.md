---
nav_exclude: true
---
# XmStringUnparse parse-table validation

`XmStringUnparse` now rejects a nonzero parse count with a null table, and its
component matcher skips null mapping entries. Invalid parse-table inputs no
longer dereference mapping state while valid tables behave as before.

The regression check is
`tests/automated/check-xmstring-unparse-table.py`.
