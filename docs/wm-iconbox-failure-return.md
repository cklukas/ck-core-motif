---
nav_exclude: true
---
# WmIconBox creation failure

The icon-box insertion path calls `MWarning(char *, char *)`, but its
allocation-failure branch supplied only one argument and returned `NULL` from
a `Boolean` function.  The branch now passes the translated diagnostic as a
`%s` message and returns `False`, preserving the failure behavior while
compiling correctly with current C diagnostics.

The regression check is
`tests/automated/check-wm-iconbox-failure-return.py`.
