---
nav_exclude: true
---
# FileSelectionBox update-resource rollback

`FileSelectionBoxUpdate` now prepares replacement copies of the Directory and
Pattern resources before releasing the current values.  If an `XmStringCopy`
allocation fails, the current search resources remain intact and the existing
values continue to drive filter-text refresh and subsequent searches.  An
explicit NULL value still clears the corresponding resource.

The focused regression is
`tests/automated/check-filesb-update-resource-rollback.py`.
