---
nav_exclude: true
---
# Empty ASN.1 string-table allocation

When a Motif compound-string text property contains no items,
`XmCvtTextPropertyToXmStringTable` still returns an allocated empty table.  The
allocation is now checked and reports `BadAlloc` with the application lock
balanced if it fails.

The regression check is
`tests/automated/check-txtpropcv-empty-asn1-table.py`.
