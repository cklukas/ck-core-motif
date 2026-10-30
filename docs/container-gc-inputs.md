---
nav_exclude: true
---
# Container graphics-context input validation

Graphics-context refresh now returns safely when no Container widget is
provided, before releasing or allocating graphics contexts. Valid GC refresh
behavior is unchanged.

The source contract is checked by
`tests/automated/check-container-gc-inputs.py`.
