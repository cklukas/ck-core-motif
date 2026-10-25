---
nav_exclude: true
---
# FontSelector list-refresh rollback

`XmFontSelector` now checks every temporary family and size string generated
during list refresh.  If a copy or localized-string allocation fails, all
partial entries are released and the child list is not updated with a NULL
entry.  Successful refreshes retain the existing filtering and selection
behavior.

The focused regression is
`tests/automated/check-fontselector-list-refresh-rollback.py`.
