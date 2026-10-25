---
nav_exclude: true
---
# XmParseMapping substitute replacement

`XmParseMappingSetValues` now replaces an existing substitute transactionally:
the old compound string is freed after a new copy succeeds. Clearing a
substitute is supported, while copy failure preserves the previous value
instead of leaking or partially updating it.

The regression check is
`tests/automated/check-xmstring-mapping-substitute.py`.
