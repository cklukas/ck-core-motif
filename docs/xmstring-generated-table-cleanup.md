---
nav_exclude: true
---
# Generated XmString parse-table cleanup

The internal generated parse table now releases each temporary newline
substitute and rolls back the cached table if either mapping allocation fails.
This prevents a permanent null-entry table and removes the repeated-call leak.

The regression check is
`tests/automated/check-xmstring-generated-table-cleanup.py`.
