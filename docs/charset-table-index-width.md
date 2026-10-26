---
nav_exclude: true
---
# Charset-table population index width

The `XmRCharSetTable` converter counts entries as an `unsigned int`, matching
the bounds used before its combined allocation is made.  Population now uses
that same width instead of casting the count to `int`; a large representable
table therefore cannot make the loop condition negative or skip entries.

The change is internal and preserves the resource format and all ordinary
conversion results.  `tests/automated/check-charset-table-index-width.py`
protects the source-level contract.
