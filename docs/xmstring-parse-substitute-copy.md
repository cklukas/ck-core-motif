---
nav_exclude: true
---
# XmString parser substitute-copy failure

When a non-procedure parse mapping has a substitute, `XmStringParseText` now
aborts if copying that substitute fails. The partial result is freed rather
than silently omitting the requested replacement; parse procedures that
intentionally return no insertion remain unchanged.

The regression check is
`tests/automated/check-xmstring-parse-substitute-copy.py`.
