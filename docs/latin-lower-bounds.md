---
nav_exclude: true
---
# Bounded Latin-1 resource conversion

`XmCopyISOLatin1Lowered` remains available for callers that own an adequately
sized destination. Resource converters that lower arbitrary input into fixed
`BUFSIZ` or action buffers now use the bounded internal variant
`XmCopyISOLatin1LoweredN`, which always terminates the destination and safely
truncates unrepresentable input.

The focused source check is
`tests/automated/check-latin-lower-bounds.py`.
