---
nav_exclude: true
---
# Mrm name-registration allocation handling

Global and hierarchy-scoped name registration now reject a nonzero count with a
null registration list and reject vector sizes that exceed either host
`size_t` or Xt's `Cardinal` allocation width. Both temporary name/value vector
allocations are checked.
Failure paths release any vector already allocated and unlock the Mrm process
state before returning.

The source-level regression is
`tests/automated/check-mrm-register-names-allocation.py`.
