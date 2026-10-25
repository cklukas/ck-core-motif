---
nav_exclude: true
---

# Auto-suggest string-copy allocation guards

`XmAutoSuggestBox` now checks the temporary text buffer before constructing a
copied `XmString`. Allocation failure returns `NULL` instead of passing a null
pointer to the string constructor; the existing empty-string fallback remains
unchanged when it can be allocated.

The focused regression check is
`tests/automated/check-auto-suggest-string-copy-guards.py`.
