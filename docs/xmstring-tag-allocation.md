---
nav_exclude: true
---
# XmString tag-cache allocation

`_XmStringIndexCacheTag` now checks allocation of a new cached tag before
copying its bytes. Failure releases the Motif process lock and returns the
existing cache-failure index instead of dereferencing a null buffer.

`tests/automated/check-xmstring-tag-allocation.py` protects this contract.
