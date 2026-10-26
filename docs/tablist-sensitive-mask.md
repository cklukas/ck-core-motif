---
nav_exclude: true
---
# Tab-list sensitivity mask

`XmTabbedStackListInsert` now tests `XmTAB_SENSITIVE` with a bitwise mask,
matching the other tab-list attribute paths. The previous multiplication test
could treat unrelated mask bits as a request to update sensitivity, so a tab
insert could silently copy `attributes->sensitive` when the caller had not
selected that attribute. The public mask and attribute contracts are unchanged.

The focused regression is
`tests/automated/check-tablist-sensitive-mask.py`.
