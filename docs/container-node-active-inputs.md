---
nav_exclude: true
---
# Container traversal-node activity validation

`NodeIsActive` now rejects nodes without widgets, parents, or required outline
constraints before querying management or visibility state. Traversal helpers
therefore skip malformed or teardown-time nodes consistently.

The source contract is checked by
`tests/automated/check-container-node-active-inputs.py`.
