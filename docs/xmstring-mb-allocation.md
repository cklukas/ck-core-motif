---
nav_exclude: true
---
# XmString multibyte conversion allocation

`_XmGetMBStringFromXmString` now checks the result buffer in both its
component and segment conversion paths. A failed allocation releases the
active component data and conversion context before returning `NULL`; normal
conversion output is unchanged.

`tests/automated/check-xmstring-mb-allocation.py` protects this contract.
