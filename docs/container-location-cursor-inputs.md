---
nav_exclude: true
---
# Container location-cursor input validation

Location-cursor restoration now returns safely when a child has no parent
Container before querying focus or requesting traversal. Valid focus restoration
is unchanged.

The source contract is checked by
`tests/automated/check-container-location-cursor-inputs.py`.
