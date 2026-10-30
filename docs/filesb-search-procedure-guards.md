---
nav_exclude: true
---

# FileSelectionBox search procedure guards

`FileSelectionBoxUpdate` now checks the qualify and directory-search procedure
slots before invoking them. The optional file-search procedure is invoked only
when present. This prevents partially initialized or customized dialogs from
calling through null procedure pointers while preserving normal search flow.

The focused source contract is checked by
`tests/automated/check-filesb-search-procedure-guards.py`.
