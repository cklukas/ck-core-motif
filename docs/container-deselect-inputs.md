---
nav_exclude: true
---
# Container deselection input validation

Deselect-all now returns safely when no Container widget is supplied, before
reading selection counters or traversing nodes. Valid selection clearing is
unchanged.

The source contract is checked by
`tests/automated/check-container-deselect-inputs.py`.
