---
nav_exclude: true
---

# FileSelectionBox programmatic search widget validation

`XmFileSelectionDoSearch` now verifies that its widget argument is a
FileSelectionBox subclass before entering the application lock or reading
private dialog state. Null and wrong-class callers return safely; valid
programmatic searches are unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-do-search-class.py`.
