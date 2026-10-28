---
nav_exclude: true
---

# XmString-table parser input validation

The internal `GetNextXmString` parser now rejects missing state pointers before
reading the input cursor, output component, or delimiter state. This keeps a
malformed conversion request fail-closed while preserving the existing parser
and rollback behavior for valid XmString tables.

The focused source-contract check is
`tests/automated/check-string-table-converter-allocation.py`.
