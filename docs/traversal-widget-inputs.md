---
nav_exclude: true
---

# Traversal widget input validation

Traversal lifecycle hooks now return safely when their widget argument is
missing, before invoking Xt widget macros or application-context helpers.
Valid navigation initialization, set-values, managed-change, and resize paths
remain unchanged.

The focused regression check is
`tests/automated/check-traversal-widget-inputs.py`.
