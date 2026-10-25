# ASN.1 compound-string output

The Motif compound-string conversion path now checks its output buffer before
serializing ASN.1 data. Allocation failure returns `BadAlloc` with the
application lock balanced instead of writing through null storage.

The regression check is
`tests/automated/check-txtpropcv-asn1-allocation.py`.
