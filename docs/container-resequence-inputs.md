---
nav_exclude: true
---
# Container node resequencing input validation

Node resequencing now returns safely for a missing Container or unavailable
entry-parent constraint and skips malformed nodes or missing child constraints.
Valid position-index resequencing is unchanged.

The source contract is checked by
`tests/automated/check-container-resequence-inputs.py`.
