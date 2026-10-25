---
nav_exclude: true
---
# RowColumn tear-off title rollback

`XmRowColumn` now copies a non-NULL tear-off title before releasing the
current title during `SetValues`.  Failed copies keep the existing menu title;
setting the title to NULL still clears it intentionally.

The focused regression is
`tests/automated/check-rowcolumn-tearoff-title-rollback.py`.
