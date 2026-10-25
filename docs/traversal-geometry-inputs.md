---
nav_exclude: true
---

# Traversal geometry input validation

Visibility and rectangle utility helpers now handle missing widgets or
rectangle pointers safely: visibility reports `False`, intersection reports no
intersection, empty-rectangle checks treat null as empty, and void helpers
return without writing. Valid geometry calculations are unchanged.

The focused regression check is
`tests/automated/check-traversal-geometry-inputs.py`.
