---
nav_exclude: true
---

# Traversal query input validation

Obscured-traversal callbacks, focus-path clearing, and focus-policy lookup now
return safely when their required widget or ancestry is absent. Valid traversal
queries and focus-path updates remain unchanged.

The focused regression check is
`tests/automated/check-traversal-query-inputs.py`.
