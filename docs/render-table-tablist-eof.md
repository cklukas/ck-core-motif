---
nav_exclude: true
---
# Render-table tab-list EOF handling

Render-table tab-list import now rejects EOF or a newline before the closing
bracket or separator.  Malformed properties can no longer leave either token
loop rereading EOF indefinitely; valid tab lists retain their existing parsing
and resource behavior.

The regression check is
`tests/automated/check-render-table-tablist-eof.py`.
