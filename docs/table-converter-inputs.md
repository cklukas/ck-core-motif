---
nav_exclude: true
---

# Table converter input validation

String-table, `XmString`-table, and cardinal-list converters now reject missing
source or destination `XrmValue` structures before parsing or allocation.
Valid table parsing and output-size negotiation remain unchanged.

The focused regression check is
`tests/automated/check-table-converter-inputs.py`.
