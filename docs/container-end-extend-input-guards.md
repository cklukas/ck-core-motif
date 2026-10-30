---
nav_exclude: true
---
# Container end-extend input guards

The Container end-extend action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button activation
and primary-selection timestamp handling from malformed action calls while
leaving normal range-selection completion unchanged.

The source contract is checked by
`tests/automated/check-container-end-extend-input-guards.py`.
