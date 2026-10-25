---
nav_exclude: true
---
# Mrm widget-reference input validation

`Urm__CW_AddWRef` now validates its resource context and widget-name pointer
before reading the context buffer or measuring the name. Valid references
retain their existing storage and lookup behavior; malformed input returns
`MrmFAILURE` instead of faulting during reference construction.

The source-level regression is `tests/automated/check-mrm-wref-input.py`.
