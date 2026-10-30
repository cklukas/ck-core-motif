---
nav_exclude: true
---
# Container cancel input guards

The Container cancel action now returns safely when its event or
parameter-count pointer is missing.  This protects parent-gadget cancellation
and primary-selection timestamp handling from malformed action calls while
leaving normal cancellation behavior unchanged.

The source contract is checked by
`tests/automated/check-container-cancel-input-guards.py`.
