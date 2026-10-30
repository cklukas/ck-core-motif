---
nav_exclude: true
---
# Container select input guards

The Container select action now returns safely when its event or
parameter-count pointer is missing.  This protects outline-button activation
and keyboard selection dispatch from malformed programmatic action calls while
leaving normal selection behavior unchanged.

The source contract is checked by
`tests/automated/check-container-select-input-guards.py`.
