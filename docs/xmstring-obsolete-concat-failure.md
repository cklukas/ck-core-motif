---
nav_exclude: true
---
# Legacy XmString constructor failure handling

`XmStringLtoRCreate` and `XmStringSegmentCreate` now stop and return `NULL`
when a component or separator concatenation fails. Previously a failed
allocation could be followed by another concat, allowing the constructor to
return a later fragment while silently losing the earlier text. The public
legacy APIs and successful-path behavior remain unchanged.

The regression check is
`tests/automated/check-xmstring-obsolete-concat-failure.py`.
