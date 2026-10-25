---
nav_exclude: true
---
# XmString implicit direction-pattern allocation

`XmStringParseText` now checks creation of its cached implicit direction
mapping. If allocation fails, the initial result is freed and parsing returns
`NULL` before the missing mapping can be dereferenced.

The regression check is
`tests/automated/check-xmstring-direction-pattern.py`.
