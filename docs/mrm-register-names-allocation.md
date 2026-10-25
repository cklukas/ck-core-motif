---
nav_exclude: true
---
# Mrm name-registration allocation handling

Global and hierarchy-scoped name registration now reject a nonzero count with a
null registration list and check both temporary name/value vector allocations.
Failure paths release any vector already allocated and unlock the Mrm process
state before returning.

The source-level regression is
`tests/automated/check-mrm-register-names-allocation.py`.
