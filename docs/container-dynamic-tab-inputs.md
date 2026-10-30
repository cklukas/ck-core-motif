---
nav_exclude: true
---
# Dynamic Container tab-list input validation

Dynamic detail-tab generation now returns safely when called without a
Container widget. Existing tab sizing, allocation, and fallback behavior for
valid widgets is unchanged.

The source contract is checked by
`tests/automated/check-container-dynamic-tab-inputs.py`.
