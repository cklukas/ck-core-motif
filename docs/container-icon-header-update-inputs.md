---
nav_exclude: true
---
# Container icon-header update validation

Icon-header updates now return safely when the header or heading data is
missing, and restore the internal update flag on both failure and success.
Valid heading updates retain their existing labels and detail counts.

The source contract is checked by
`tests/automated/check-container-icon-header-update-inputs.py`.
