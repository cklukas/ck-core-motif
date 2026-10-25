---
nav_exclude: true
---
# XmStringGetLtoR output initialization

`XmStringGetLtoR` now clears its text output as soon as the output pointer is
validated. Null strings or tags therefore leave `*text` as `NULL` instead of
returning with a stale caller-owned pointer.

The regression check is
`tests/automated/check-xmstring-getltor-output.py`.
