---
nav_exclude: true
---
# External XIC reconstruction allocation

When an application-created XIC is first seen, Motif reconstructs a private
record for it.  That record is now checked before initialization; allocation
failure returns `NULL` and leaves the existing XIC untouched.  Successful
reconstruction and input-method behavior are unchanged.

`tests/automated/check-xim-recreate-allocation.py` protects this contract.
