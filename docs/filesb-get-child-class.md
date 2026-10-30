---
nav_exclude: true
---

# FileSelectionBox child lookup widget validation

`XmFileSelectionBoxGetChild` now verifies that its widget argument is a
FileSelectionBox subclass before entering the application lock or reading
private child fields. Null and wrong-class callers return `NULL`; valid child
lookups are unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-get-child-class.py`.
