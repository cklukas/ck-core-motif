---
nav_exclude: true
---

# Auto-suggest string-copy allocation guards

`XmAutoSuggestBox` now copies retained item labels directly as `XmString`
values, preserving compound-string charset segments and multilingual text.
The separate text conversion used for filtering and callbacks remains
allocation-checked.

The focused regression check is
`tests/automated/check-auto-suggest-string-copy-guards.py`.
