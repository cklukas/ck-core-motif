---
nav_exclude: true
---
# XmStringNConcat allocation failure

The legacy `XmStringNConcat` wrapper now checks the result of its initial
`XmStringConcat` before measuring and copying it. If concatenation fails, it
returns `NULL` immediately instead of passing a null result to the length and
bounded-copy helpers.

The regression check is
`tests/automated/check-xmstring-nconcat-failure.py`.
