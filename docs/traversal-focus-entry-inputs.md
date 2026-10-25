---
nav_exclude: true
---

# Traversal focus-entry input validation

Focus validation, destruction, manager traversal, and focus-moved callback
helpers now return safely when required widgets are absent. Normal traversal
and callback dispatch are unchanged for valid focus transitions.

The focused regression check is
`tests/automated/check-traversal-focus-entry-inputs.py`.
