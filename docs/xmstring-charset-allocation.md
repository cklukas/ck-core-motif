---
nav_exclude: true
---
# XmString default charset allocation

The default compound-string charset is cached after parsing the locale. Its
length is checked against the cached `int` field and Xt's `Cardinal` width
before allocation. If sizing or allocation fails, lookup returns
`XmFALLBACK_CHARSET` while leaving initialization incomplete so a later call
can retry. This prevents narrow conversion and null writes while preserving the
normal locale-derived value when memory is available.

`tests/automated/check-xmstring-charset-allocation.py` protects this contract.
