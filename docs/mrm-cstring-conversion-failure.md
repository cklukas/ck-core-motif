# Mrm compound-string conversion failures

Mrm converts serialized compound-string literals through
`XmCvtByteStreamToXmString`.  Invalid ASN.1 data can make that conversion
return `NULL`.  Mrm now returns `MrmFAILURE` instead of passing that null value
to widget construction or recording it for later cleanup.

The conversion guard is covered by
`tests/automated/check-mrm-conversion-allocation.py`.
