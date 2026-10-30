---
nav_exclude: true
---

# Empty plain-text import

When an empty plain-text property is converted, the generated empty
`XmString` is now checked before the result table is returned. Allocation
failure releases the table and returns `BadAlloc` instead of reporting success
with a NULL entry.

The focused regression check is
`tests/automated/check-txtpropcv-empty-generation.py`.
