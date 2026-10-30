---
nav_exclude: true
---
# Container tree-helper input validation

Recursive depth reset now skips malformed child nodes or missing constraints.
Descendant checks return false for missing widgets or parent constraint records.
Valid tree depth and descendant behavior are unchanged.

The source contract is checked by
`tests/automated/check-container-tree-helper-inputs.py`.
