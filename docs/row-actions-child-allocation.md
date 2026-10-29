---
nav_exclude: true
---
# RowActions child allocation

RowActions creates one push button per configured action.  Its preferred-size
and layout paths now tolerate a missing button, preserving safe redraw and
reveal behavior even while a failed child allocation is being recovered.

The contract is protected by
`tests/automated/check-row-actions-child-failure-guards.py`.
