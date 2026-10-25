---
nav_exclude: true
---
# ASN.1 compound-string output allocation

The `XmSTYLE_COMPOUND_STRING` conversion path now checks its final ASN.1
output buffer before converting items into it.  Allocation failure returns
`BadAlloc` with the application lock balanced instead of dereferencing a null
buffer.

The regression check is
`tests/automated/check-txtpropcv-asn1-allocation.py`.
