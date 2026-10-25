---
nav_exclude: true
---
# UTF-8 to UCS-2 allocation

`_XmUtf8ToUcs2` validates the requested buffer size before allocating the
temporary `XChar2b` array. It now also handles an allocation failure by
returning `NULL` and resetting the output length, rather than dereferencing a
null buffer while converting input bytes.

Successful conversion and invalid-byte replacement remain unchanged.
