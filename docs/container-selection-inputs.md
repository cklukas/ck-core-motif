---
nav_exclude: true
---
# Container selection input validation

Container selection callbacks now return safely for a missing container, and
selected-item collection skips malformed nodes or missing constraint records
instead of dereferencing them. Valid selection behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-selection-inputs.py`.
