---
nav_exclude: true
---
# XmList item mutation rollback

List insertion now checks every `XmStringCopy`.  If a copy fails, already
copied entries are released, the pre-existing items are moved back into place,
and the operation reports failure without publishing a partial list.  The
replacement path copies the new item before freeing the old value, so a failed
copy leaves both the primary and selected-item representations intact.

The insertion allocation is also checked against Xt's `Cardinal` byte range.
The focused regression is `tests/automated/check-list-item-mutation-rollback.py`.
