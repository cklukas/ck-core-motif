---
nav_exclude: true
---

# Parse-table cleanup

`XmParseTableFree` treats a `NULL` parse-table pointer as an empty table and
returns safely, even when a nonzero count is supplied. Existing tables still
have every mapping freed and are released as before.

The focused regression check is
`tests/automated/check-parse-table-free-null.py`.
