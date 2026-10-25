# RowColumn tear-off title initialization rollback

`XmRowColumn` now checks the initial `XmStringCopy` of its tear-off title.
When that copy fails, initialization installs an empty compound string rather
than propagating a NULL title into menu construction and later title handling.
The existing `SetValues` rollback behavior remains unchanged.

The focused regression is
`tests/automated/check-rowcolumn-tearoff-title-init-rollback.py`.
