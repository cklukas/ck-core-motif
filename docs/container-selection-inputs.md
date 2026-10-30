---
nav_exclude: true
---
# Container selection input validation

The shared selection-marking helpers now reject null children and missing
Container constraints before reading selection state or sensitivity. Bulk
selection operations therefore tolerate malformed or teardown-time child
entries without changing valid selection behavior.

The source contract is checked by
`tests/automated/check-container-selection-inputs.py`.
