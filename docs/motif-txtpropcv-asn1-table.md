---
nav_exclude: true
---
# Non-empty ASN.1 string-table allocation

The non-empty Motif compound-string branch now checks its second-pass
`XmStringTable` allocation against Xt's `Cardinal` byte range before decoding
ASN.1 items.  Allocation failure returns `BadAlloc` with the application lock
balanced instead of entering the conversion loop with null storage.

The regression check is
`tests/automated/check-txtpropcv-asn1-table.py`.
