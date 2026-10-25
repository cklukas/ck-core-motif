---
nav_exclude: true
---

# Row-actions string-copy allocation guards

`XmRowActions` now checks the temporary text buffer before creating a copied
`XmString`. Allocation failure returns `NULL` to the caller instead of passing a
null pointer into the string constructor.

The focused regression check is
`tests/automated/check-row-actions-string-copy-guards.py`.
