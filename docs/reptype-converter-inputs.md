---
nav_exclude: true
---

# Representation-type converter input validation

The forward and reverse representation-type converters now reject missing Xt
argument/value structures and unresolved record IDs before dereferencing them.
Valid conversions and warning behavior for known records remain unchanged.

The focused regression check is
`tests/automated/check-reptype-converter-inputs.py`.
