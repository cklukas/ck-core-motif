---
nav_exclude: true
---
# Container node lifecycle input validation

Node creation and insertion now reject missing children or constraints,
normalize an unavailable parent constraint to a root entry, and keep view
traversal from calling into malformed nodes. Valid tree construction and view
changes are unchanged.

The source contract is checked by
`tests/automated/check-container-node-lifecycle-inputs.py`.
