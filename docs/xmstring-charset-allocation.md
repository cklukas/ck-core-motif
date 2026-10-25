---
nav_exclude: true
---
# XmString default charset allocation

The default compound-string charset is cached after parsing the locale.  If
its cache allocation fails, lookup now returns `XmFALLBACK_CHARSET` while
leaving initialization incomplete so a later call can retry.  This prevents a
null write or comparison and preserves the normal locale-derived value when
memory is available.

`tests/automated/check-xmstring-charset-allocation.py` protects this contract.
