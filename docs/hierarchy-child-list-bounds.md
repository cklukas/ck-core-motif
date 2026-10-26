# Hierarchy child-list allocation bounds

Hierarchy's dynamic child-constraint list now checks the computed byte count
against Xt's `Cardinal` limit after the existing host-size multiplication
check.  Oversized growth requests return without passing a truncated size to
`XtRealloc`.

The focused source-contract check is:

    python3 tests/automated/check-hierarchy-child-list-bounds.py
