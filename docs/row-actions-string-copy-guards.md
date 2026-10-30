---
nav_exclude: true
---

# Row-actions string-copy allocation guards

`XmRowActions` now copies retained item labels and shortcuts directly as
`XmString` values, preserving compound-string segments. Allocation failure
returns the empty-string fallback or `NULL` as appropriate without a text
conversion round trip.

The focused regression check is
`tests/automated/check-row-actions-string-copy-guards.py`.
