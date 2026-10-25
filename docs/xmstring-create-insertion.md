---
nav_exclude: true
---
# XmString creation insertion failure

The non-optimized `_XmStringNCreate` path now verifies that its initial
segment insertion succeeded. Allocation failure returns `NULL` after freeing
the partial string instead of exposing an empty or malformed compound string.

The regression check is
`tests/automated/check-xmstring-create-insertion.py`.
